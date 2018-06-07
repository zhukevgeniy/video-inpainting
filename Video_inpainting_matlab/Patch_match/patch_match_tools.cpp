//this function defines the functions which are tools used for the
//spatio-temporal patch_match algorithm


#include "patch_match_tools.h"

//check if the displacement values have already been used
template <class T>
bool check_already_used_patch( nTupleVolume<T> *dispField, int x, int y, int t, int dispX, int dispY, int dispT)
{
    
    if ( (((int)dispField->get_value(x,y,t,0)) == dispX) && 
            (((int)dispField->get_value(x,y,t,1)) == dispY) &&
            (((int)dispField->get_value(x,y,t,2)) == dispT)
            )
        return 1;
    else
        return 0;
    
}

//check if the pixel is occluded
template <class T>
int check_is_occluded( nTupleVolume<T> *imgVolOcc, int x, int y, int t)
{
	if (imgVolOcc->xSize == 0)
		return 0;
	if ( (imgVolOcc->get_value(x,y,t,0)) > 0)
		return 1;
	else
		return 0;
}

template <class T>
float calclulate_patch_error(nTupleVolume<T> *departVolume,nTupleVolume<T> *arrivalVolume,nTupleVolume<T> *dispField, nTupleVolume<T> *occVol,
		int xA, int yA, int tA, float minError, const parameterStruct *params)
{
	int xB, yB, tB;
	float errorOut;

	xB = (xA) + (int)dispField->get_value(xA,yA,tA,0);
	yB = (yA) + (int)dispField->get_value(xA,yA,tA,1);
	tB = (tA) + (int)dispField->get_value(xA,yA,tA,2);

	errorOut = ssd_patch_measure(departVolume, arrivalVolume,dispField,occVol, xA, yA, tA, xB, yB, tB, minError, params);
	return(errorOut);
}

template <class T>
int check_disp_field(nTupleVolume<T> *dispField, nTupleVolume<T> *departVolume, nTupleVolume<T> *arrivalVolume, nTupleVolume<T> *occVol, const parameterStruct *params)
{
	int dispValX,dispValY,dispValT,hPatchSizeX,hPatchSizeY,hPatchSizeT;
	int xB,yB,tB;
	int i,j,k,returnVal;

	hPatchSizeX = (int)floor((float)((departVolume->patchSizeX)/2));	//half the patch size
	hPatchSizeY = (int)floor((float)((departVolume->patchSizeY)/2));	//half the patch size
	hPatchSizeT = (int)floor((float)((departVolume->patchSizeT)/2));	//half the patch size

	returnVal = 0;
	for (k=hPatchSizeT; k< ((dispField->tSize) -hPatchSizeT); k++)
		for (j=hPatchSizeY; j< ((dispField->ySize) -hPatchSizeY); j++)
			for (i=hPatchSizeX; i< ((dispField->xSize) -hPatchSizeX); i++)
			{
				dispValX = (int)dispField->get_value(i,j,k,0);
				dispValY = (int)dispField->get_value(i,j,k,1);
				dispValT = (int)dispField->get_value(i,j,k,2);


				xB = dispValX + i;
				yB = dispValY + j;
				tB = dispValT + k;

				if ( check_in_inner_boundaries(arrivalVolume, xB, yB, tB,params) == 0)
				{
					MY_PRINTF("Error, the displacement is incorrect.\n");
					MY_PRINTF("xA : %d\n yA : %d\n tA : %d\n",i,j,k);
					MY_PRINTF(" dispValX : %d\n dispValY : %d\n dispValT : %d\n",dispValX,dispValY,dispValT);
					MY_PRINTF(" xB : %d\n yB : %d\n tB : %d\n",xB,yB,tB);
					returnVal= -1;
				}
				else if (check_is_occluded(occVol,xB,yB,tB) == 1)
				{
					MY_PRINTF("Error, the displacement leads to an occluded pixel.\n");
					MY_PRINTF(" xB : %d\n yB : %d\n tB : %d\n",xB,yB,tB);
					returnVal= -1;
				}
			}
	return(returnVal);

}

template <class T>
void patch_match_full_search(nTupleVolume<T> *dispField, nTupleVolume<T> *imgVolA,nTupleVolume<T> *imgVolB,
        nTupleVolume<T> *occVol, nTupleVolume<T> *modVol, const parameterStruct *params)
{
    float minSSD,ssdTemp;
    int hPatchSizeX, hPatchSizeY, hPatchSizeT;
    int i,j,k,ii,jj,kk;
    int bestX, bestY, bestT;
    
    hPatchSizeX = (int)floor((float)((dispField->patchSizeX)/2));	//half the patch size
	hPatchSizeY = (int)floor((float)((dispField->patchSizeY)/2));	//half the patch size
	hPatchSizeT = (int)floor((float)((dispField->patchSizeT)/2));	//half the patch size
            
    //#pragma omp parallel for shared(dispField, occVol, imgVolA, imgVolB) private(kk,jj,ii,minSSD,ssdTemp,bestX,bestY,bestT,i,j,k)
	for (k=hPatchSizeT; k< ((dispField->tSize) -hPatchSizeT); k++)
		for (j=hPatchSizeY; j< ((dispField->ySize) -hPatchSizeY); j++)
        {
			for (i=hPatchSizeX; i< ((dispField->xSize) -hPatchSizeX); i++)
            {
                minSSD = FLT_MAX;
                if (modVol->xSize >0)
                    if (modVol->get_value(i,j,k,0) == 0)   //if we don't want to modify this match
                       continue;
                //search for the best match
                for (kk=hPatchSizeT; kk< ((dispField->tSize) -hPatchSizeT); kk++)
                    for (jj=hPatchSizeY; jj< ((dispField->ySize) -hPatchSizeY); jj++)
                        for (ii=hPatchSizeX; ii< ((dispField->xSize) -hPatchSizeX); ii++)
                        {
                            if (occVol->xSize >0)
                                if (check_is_occluded(occVol,ii,jj,kk))   //if this pixel is occluded, continue
                                    continue;

                            ssdTemp = ssd_patch_measure<T>(imgVolA, imgVolB, dispField,occVol, i, j, k,ii, jj, kk, minSSD,params);
                            if ( (ssdTemp != -1) && (ssdTemp <= minSSD))   //we have a new best match
                            {
                                minSSD = ssdTemp;
                                bestX = ii - i;
                                bestY = jj - j;
                                bestT = kk - k;
                            }
                        }
                dispField->set_value(i,j,k,0,(T)bestX);
                dispField->set_value(i,j,k,1,(T)bestY);
                dispField->set_value(i,j,k,2,(T)bestT);
                dispField->set_value(i,j,k,3,minSSD);
            }
        }

}

template <class T>
void initialise_displacement_field(nTupleVolume<T> *dispField, nTupleVolume<T> *departVolume, 
            nTupleVolume<T> *arrivalVolume, nTupleVolume<T> *firstGuessVolume, nTupleVolume<T> *occVol, const parameterStruct *params)
{
	//declarations
	int i,j,k, xDisp, yDisp, tDisp, hPatchSizeX, hPatchSizeY, hPatchSizeT, hPatchSizeCeilX, hPatchSizeCeilY, hPatchSizeCeilT;
	int xMin,xMax,yMin,yMax,tMin,tMax;
	int xFirst,yFirst,tFirst;
	int isNotOcc, currShift,currLinInd;
    float ssdTemp;

	hPatchSizeX = (int)floor(((float)arrivalVolume->patchSizeX)/2);
	hPatchSizeY = (int)floor(((float)arrivalVolume->patchSizeY)/2);
	hPatchSizeT = (int)floor(((float)arrivalVolume->patchSizeT)/2);

	hPatchSizeCeilX = (int)ceil(((float)arrivalVolume->patchSizeX)/2);
	hPatchSizeCeilY = (int)ceil(((float)arrivalVolume->patchSizeY)/2);
	hPatchSizeCeilT = (int)ceil(((float)arrivalVolume->patchSizeT)/2);

	for (i=0; i< (dispField->xSize); i++)
		for (j=0; j< (dispField->ySize); j++)
			for (k=0; k< (dispField->tSize); k++)
			{
				isNotOcc = 0;
                //if there is a valid first guess
                while(isNotOcc == 0)
                {
                    //if there is a first guess, and it is in the inner boundaries, and respects the minimum shift distance
                    if ( (firstGuessVolume->xSize >0) && (check_in_inner_boundaries(arrivalVolume,i+(int)firstGuessVolume->get_value(i,j,k,0),j+
                        (int)firstGuessVolume->get_value(i,j,k,1),k+(int)firstGuessVolume->get_value(i,j,k,2),params ) )
                        )
                    {
                        //if it is not occluded, we take the initial first guess and continue
                        if (!check_is_occluded(occVol,i+(int)firstGuessVolume->get_value(i,j,k,0),j+(int)firstGuessVolume->get_value(i,j,k,1),
                            k+(int)firstGuessVolume->get_value(i,j,k,2)) )
                        {
                            xDisp = (int)firstGuessVolume->get_value(i,j,k,0);
                            yDisp = (int)firstGuessVolume->get_value(i,j,k,1);
                            tDisp = (int)firstGuessVolume->get_value(i,j,k,2);
                            isNotOcc = 1;
                            continue;
                        }
                        else    //otherwise, we set up the calculation of a random initial starting point, centred on the initial guess
                        {
                            xFirst = i+(int)firstGuessVolume->get_value(i,j,k,0);
                            yFirst = j+(int)firstGuessVolume->get_value(i,j,k,1);
                            tFirst = k+(int)firstGuessVolume->get_value(i,j,k,2);
                            xMin = max_int(xFirst-params->w,hPatchSizeX);
                            xMax = min_int(xFirst+params->w,arrivalVolume->xSize - hPatchSizeX -1);
                            yMin = max_int(yFirst-params->w,hPatchSizeY);
                            yMax = min_int(yFirst+params->w,arrivalVolume->ySize - hPatchSizeY -1);
                            tMin = max_int(tFirst-params->w,hPatchSizeT);
                            tMax = min_int(tFirst+params->w,arrivalVolume->tSize - hPatchSizeT -1);
                        }
                    }
                    else    //by default, set the displacement to float_max
                    {
                        dispField->set_value(i,j,k,0,(T)FLT_MAX);
                        dispField->set_value(i,j,k,1,(T)FLT_MAX);
                        dispField->set_value(i,j,k,2,(T)FLT_MAX);
                    }   
                    if (arrivalVolume->xSize == arrivalVolume->patchSizeX)	//special case where the patch size is the size of the dimension
                    {
                        xDisp = 0;
                    }
                    else{
                        if ( (dispField->get_value(i,j,k,0) == FLT_MAX) || (firstGuessVolume->xSize == 0))  //default behaviour
                        {
                            xDisp = ((rand()%( (arrivalVolume->xSize) -2*hPatchSizeCeilX-1)) + hPatchSizeX)-i;
                        }
                        else    //based on an initial guess
                        {
                            xDisp = (int)(round_float(rand_float_range((float)(xMin),(float)(xMax))) - i);
                        }
                    }
                    if (arrivalVolume->ySize == arrivalVolume->patchSizeY)	//special case where the patch size is the size of the dimension
                    {
                        yDisp = 0;
                    }
                    else{
                        if ( (dispField->get_value(i,j,k,1) == FLT_MAX) || (firstGuessVolume->xSize == 0))  //default behaviour
                        {
                            yDisp = ((rand()%( (arrivalVolume->ySize) -2*hPatchSizeCeilY-1)) + hPatchSizeY)-j;
                        }
                        else    //based on an initial guess
                        {
                            yDisp = (int)(round_float(rand_float_range((float)(yMin),(float)(yMax))) - j);
                        }
                    }
                    if (arrivalVolume->tSize == arrivalVolume->patchSizeT)	//special case where the patch size is the size of the dimension
                    {
                        tDisp = 0;
                    }
                    else{
                        if ( (dispField->get_value(i,j,k,2) == FLT_MAX) || (firstGuessVolume->xSize == 0))  //default behaviour
                        {
                            tDisp = ((rand()%( (arrivalVolume->tSize) -2*hPatchSizeCeilT-1)) + hPatchSizeT)-k;
                        }
                        else    //based on an initial guess
                        {
                            tDisp = (int)(round_float(rand_float_range((float)(tMin),(float)(tMax))) - k);
                        }
                    }
                    isNotOcc = (!(check_is_occluded(occVol,xDisp+i,yDisp+j,tDisp+k))
                             &&(check_in_inner_boundaries(arrivalVolume,xDisp+i,yDisp+j,tDisp+k,params)))
                        ;
                }
                //if everything is all right, set the displacements
                dispField->set_value(i,j,k,0, (T)(xDisp));
                dispField->set_value(i,j,k,1, (T)(yDisp));
                dispField->set_value(i,j,k,2, (T)(tDisp));

                if (check_in_inner_boundaries(departVolume,i,j,k,params))
                    ssdTemp = ssd_patch_measure<T>(departVolume, arrivalVolume, dispField,occVol, i, j, k, i+xDisp, j+yDisp, k+tDisp, -1,params);
                else
                    ssdTemp = FLT_MAX;
                dispField->set_value(i,j,k,3,(T)ssdTemp); //set the ssd error
            }
}

template <class T>
int patch_match_random_search(nTupleVolume<T> *dispField, nTupleVolume<T> *imgVolA, nTupleVolume<T> *imgVolB,
        nTupleVolume<T> *occVol, nTupleVolume<T> *modVol, const parameterStruct *params)
{
	//create random number seed
	int xRand,yRand,tRand;
	int randMinX,randMaxX,randMinY,randMaxY,randMinT,randMaxT;
	int i,j,k,z,hPatchSizeX,hPatchSizeY,hPatchSizeT, zMax;
	int xTemp,yTemp,tTemp,wTemp,wMax;
    int *xDisp, *yDisp, *tDisp;
    int isCorrectShift,currShift,currLinInd;
	float ssdTemp;
    int nbModified = 0;
    clock_t startTime;

	hPatchSizeX = (int)floor((float)((dispField->patchSizeX)/2));	//half the patch size
	hPatchSizeY = (int)floor((float)((dispField->patchSizeY)/2));	//half the patch size
	hPatchSizeT = (int)floor((float)((dispField->patchSizeT)/2));	//half the patch size
    
    xDisp = new int[1];
    yDisp = new int[1];
    tDisp = new int[1];

	//calculate the maximum z (patch search index)
    wMax = min_int(params->w, max_int(max_int(imgVolB->xSize,imgVolB->ySize),imgVolB->tSize));
	zMax = (int)ceil((float) (- (log((float)(wMax)))/(log((float)(params->alpha)))) );
    
    nTupleVolume<int> *wValues = new nTupleVolume<int>(1,zMax,1,1,imgVolA->indexing);
    //store the values of the maximum search parameters
    for (int z=0; z<zMax; z++)
    {
        wValues->set_value(z,0,0,0,
                (int)round_float((params->w)*((float)pow((float)params->alpha,z)))
                );
    }

    //#pragma omp parallel for shared(dispField, occVol, imgVolA, imgVolB,nbModified) private(ssdTemp,xTemp,yTemp,tTemp,wTemp,randMinX,randMaxX,randMinY,randMaxY,randMinT,randMaxT,xRand,yRand,tRand,i,j,k,z)
	for (k=0; k< ((dispField->tSize) ); k++)
		for (j=0; j< ((dispField->ySize) ); j++)
			for (i=0; i< ((dispField->xSize) ); i++)
			{
                if (modVol->xSize >0)
                    if (modVol->get_value(i,j,k,0) == 0)   //if we don't want to modify this match
                        continue;
				ssdTemp = dispField->get_value(i,j,k,3); //get the saved ssd value
                
                for (z=0; z<zMax; z++)	//test for different search indices
                {
                    xTemp = i+(int)dispField->get_value(i,j,k,0);	//get the arrival position of the current offset
                    yTemp = j+(int)dispField->get_value(i,j,k,1);	//get the arrival position of the current offset
                    tTemp = k+(int)dispField->get_value(i,j,k,2);	//get the arrival position of the current offset

                    wTemp = wValues->get_value(z,0,0,0);
                    // X values
                    randMinX = max_int(xTemp - wTemp,hPatchSizeX);
                    randMaxX = min_int(xTemp + wTemp,imgVolB->xSize - hPatchSizeX - 1);
                    // Y values
                    randMinY = max_int(yTemp - wTemp,hPatchSizeY);
                    randMaxY = min_int(yTemp + wTemp,imgVolB->ySize - hPatchSizeY - 1);
                    // T values
                    randMinT = max_int(tTemp - wTemp,hPatchSizeT);
                    randMaxT = min_int(tTemp + wTemp,imgVolB->tSize - hPatchSizeT - 1);

                    //new positions in the image imgB
                    xRand = rand_int_range(randMinX, randMaxX);	//random values between xMin and xMax, clamped to the sizes of the image B
                    yRand = rand_int_range(randMinY, randMaxY);	//random values between yMin and yMax, clamped to the sizes of the image B
                    tRand = rand_int_range(randMinT, randMaxT);	//random values between tMin and tMax, clamped to the sizes of the image B

                    if (check_is_occluded(occVol,xRand,yRand,tRand))
                        continue;	//the new position is occluded
                    if (check_in_inner_boundaries(imgVolB,xRand,yRand,tRand,params) == 0)
                        continue;	//the new position is not in the inner boundaries

                    ssdTemp =  ssd_patch_measure(imgVolA, imgVolB, dispField,occVol, i, j, k, xRand, yRand, tRand, ssdTemp,params);

                    if (ssdTemp != -1)	//we have a better match
                    {
                        dispField->set_value(i,j,k,0, (T)(xRand-i));
                        dispField->set_value(i,j,k,1, (T)(yRand-j));
                        dispField->set_value(i,j,k,2, (T)(tRand-k));
                        dispField->set_value(i,j,k,3, (T)(ssdTemp));

                        nbModified = nbModified+1;
                    }
                    else
                        ssdTemp = dispField->get_value(i,j,k,3); //set the saved ssd value bakc to its proper (not -1) value
                }
			}

    delete xDisp;
    delete yDisp;
    delete tDisp;
    delete wValues;
    return(nbModified);
}

//one iteration of the propagation of the patch match algorithm
template <class T>
int patch_match_propagation(nTupleVolume<T> *dispField, nTupleVolume<T> *departVolume, nTupleVolume<T> *arrivalVolume, nTupleVolume<T> *occVol,  
        nTupleVolume<T> *modVol, const parameterStruct *params, int iterationNb)
{
	//declarations
	int i,j,k, hPatchSizeX, hPatchSizeY, hPatchSizeT, *correctInd;
    int nbModified;
	int coordDispX,coordDispY, coordDispT;
	float currentError, *minVector;

	hPatchSizeX = (int)floor((float)((departVolume->patchSizeX)/2));	//half the patch size
	hPatchSizeY = (int)floor((float)((departVolume->patchSizeY)/2));	//half the patch size
	hPatchSizeT = (int)floor((float)((departVolume->patchSizeT)/2));	//half the patch size

	correctInd = (int*)malloc((size_t)sizeof(int));

	minVector = (float*)malloc((size_t)3*sizeof(float));
	
    
	//iterate over all displacements (do not go to edge, where the patches may not be defined)
    nbModified = 0;
	if (iterationNb&1)	//if we are on an odd iteration
	{
        for (k=((dispField->tSize)-1); k>= 0; k--)
			for (j=((dispField->ySize) -1); j>= 0; j--)
                for (i=((dispField->xSize) -1); i>= 0; i--)
				{
                    if (modVol->xSize >0)
                        if (modVol->get_value(i,j,k,0) == 0)   //if we don't want to modify this match
                            continue;
					
                    //calculate the error of the current displacement
					currentError = dispField->get_value(i,j,k,3);
                    
					get_min_correct_error(dispField,departVolume,arrivalVolume,occVol,
							i, j, k, iterationNb&1, correctInd,minVector,currentError,params);
					//if the best displacement is the current one. Note : we have taken into account the case
					//where none of the diplacements around the current pixel are valid
					if (*correctInd == -1)	//if the best displacement is the current one
					{
						dispField->set_value(i,j,k,3,currentError);
					}
					else	//we copy the displacement from another better one
					{
						if ((*correctInd) == 0){
							copy_pixel_values_nTuple_volume(dispField,dispField, min_int(i+1,((int)dispField->xSize)-1), j, k, i, j, k);
                            nbModified++;
                        }

						else if((*correctInd) == 1){
							copy_pixel_values_nTuple_volume(dispField,dispField, i, min_int(j+1,((int)dispField->ySize)-1), k, i, j, k);
                            nbModified++;
                        }
						else if( (*correctInd) == 2){
							copy_pixel_values_nTuple_volume(dispField,dispField, i, j, min_int(k+1,((int)dispField->tSize)-1), i, j, k);
                            nbModified++;
                        }
                        else
                            MY_PRINTF("Error, correct ind not chosen\n.");
						//now calculate the error of the patch matching
						currentError = calclulate_patch_error<T>(departVolume,arrivalVolume,dispField,occVol,i,j,k, -1,params);
						dispField->set_value(i,j,k,3,currentError);
					}
				}
	}
	else 	//if we are on an even iteration
	{
		for (k=0; k< ((dispField->tSize) ); k++)
			for (j=0; j< ((dispField->ySize) ); j++)
				for (i=0; i< ((dispField->xSize) ); i++)
				{
                    if (modVol->xSize >0)
                        if (modVol->get_value(i,j,k,0) == 0)   //if we don't want to modify this match
                            continue;
                    
					//calculate the error of the current displacement
					currentError = currentError = dispField->get_value(i,j,k,3);//calclulate_patch_error(departVolume,arrivalVolume,dispField,occVol,i,j,k, -1,params);
					
					//get the upper, left and before patch distances
					get_min_correct_error(dispField,departVolume,arrivalVolume,occVol,
							i, j, k, iterationNb&1, correctInd, minVector,currentError,params);
					//if the best displacement is the current one. Note : we have taken into account the case
					//where none of the diplacements around the current pixel are valid
					if (*correctInd == -1)
					{
						dispField->set_value(i,j,k,3,currentError);
                    }
					else	//we copy the displacement from another better one
					{
						if ( (*correctInd) == 0){
							coordDispX = (int)dispField->get_value(max_int(i-1,0),j,k,0);

							coordDispY = (int)dispField->get_value(max_int(i-1,0),j,k,1);
							coordDispT = (int)dispField->get_value(max_int(i-1,0),j,k,2);
							copy_pixel_values_nTuple_volume(dispField,dispField, max_int(i-1,0), j, k, i, j, k);
                            nbModified++;
                        }

						else if( (*correctInd) == 1){
							coordDispX = (int)dispField->get_value(i,max_int(j-1,0),k,0);
							coordDispY = (int)dispField->get_value(i,max_int(j-1,0),k,1);
							coordDispT = (int)dispField->get_value(i,max_int(j-1,0),k,2);
							copy_pixel_values_nTuple_volume(dispField,dispField, i, max_int(j-1,0), k, i, j, k);
                            nbModified++;
                        }

						else if( (*correctInd) == 2){
							copy_pixel_values_nTuple_volume(dispField,dispField, i, j, max_int(k-1,0), i, j, k);
                            nbModified++;
                        }
                        else
                            MY_PRINTF("Error, correct ind not chosen\n.");
						//now calculate the error of the patch matching
						currentError = calclulate_patch_error<T>(departVolume,arrivalVolume,dispField,occVol,i,j,k, -1,params);
						dispField->set_value(i,j,k,3,currentError);
					}
				}
	}
	free(correctInd);
	free(minVector);
    
    return(nbModified);
}

//this function returns the minimum error of the patch differences around the pixel at (i,j,k)
//and returns the index of the best position in correctInd :
// -1 : none are correct
// 0 : left/right
// 1 : upper/lower
// 2 : before/after
template <class T>
float get_min_correct_error(nTupleVolume<T> *dispField,nTupleVolume<T> *departVol,nTupleVolume<T> *arrivalVol, nTupleVolume<T> *occVol,
							int x, int y, int t, int beforeOrAfter, int *correctInd, float *minVector, float minError,
                            const parameterStruct *params)
{
	float minVal;
	int i, coordDispX,coordDispY,coordDispT;
    int dispX, dispY, dispT;

	minVal = minError;

	*correctInd = -1;	//initialise the correctInd vector to -1
    for (i=0;i<NDIMS;i++)
		minVector[i] = -1;

	if (beforeOrAfter == 0)	//we are looking left, upper, before : we are on an even iteration
	{
        dispX = (int)dispField->get_value((int)max_int(x-1,0),y,t,0); dispY = (int)dispField->get_value((int)max_int(x-1,0),y,t,1); dispT = (int)dispField->get_value((int)max_int(x-1,0),y,t,2); 
        if ( check_in_inner_boundaries(arrivalVol,x+dispX,y+dispY,t+dispT,params) && (!check_is_occluded(occVol, x+dispX, y+dispY, t+dispT)) &&
                (!check_already_used_patch( dispField, x, y, t, dispX, dispY, dispT)))
            minVector[0] = ssd_patch_measure(departVol, arrivalVol, dispField,occVol,x,y,t,x+dispX,y+dispY,t+dispT,minError,params);
        
        dispX = (int)dispField->get_value(x,(int)max_int(y-1,0),t,0); dispY = (int)dispField->get_value(x,(int)max_int(y-1,0),t,1); dispT = (int)dispField->get_value(x,(int)max_int(y-1,0),t,2); 
        if ( check_in_inner_boundaries(arrivalVol,x+dispX,y+dispY,t+dispT,params) && (!check_is_occluded(occVol, x+dispX, y+dispY, t+dispT)) && 
                (!check_already_used_patch( dispField, x, y, t, dispX, dispY, dispT)))
            minVector[1] = ssd_patch_measure(departVol, arrivalVol, dispField,occVol,x,y,t,x+dispX,y+dispY,t+dispT,minError,params);
        
        dispX = (int)dispField->get_value(x,y,(int)max_int(t-1,0),0); dispY = (int)dispField->get_value(x,y,(int)max_int(t-1,0),1); dispT = (int)dispField->get_value(x,y,(int)max_int(t-1,0),2); 
        if ( check_in_inner_boundaries(arrivalVol,x+dispX,y+dispY,t+dispT,params) && (!check_is_occluded(occVol, x+dispX, y+dispY, t+dispT)) &&
                (!check_already_used_patch( dispField, x, y, t, dispX, dispY, dispT)))
            minVector[2] = ssd_patch_measure(departVol, arrivalVol, dispField,occVol,x,y,t,x+dispX,y+dispY,t+dispT,minError,params);//
        

		for (i=0;i<NDIMS;i++)
		{
            if (minVector[i] == -1)
                continue;
            if ( minVector[i] < minVal)
			{
				switch (i){
					case 0 :
						coordDispX = (int)dispField->get_value(max_int(x-1,0),y,t,0);
						coordDispY = (int)dispField->get_value(max_int(x-1,0),y,t,1);
						coordDispT = (int)dispField->get_value(max_int(x-1,0),y,t,2);
// // 						if ( check_in_inner_boundaries(arrivalVol,x+coordDispX,y+coordDispY,t+coordDispT,params) &&
// // 							(!check_is_occluded(occVol, x+coordDispX, y+coordDispY, t+coordDispT)) &&
// // 							 check_min_shift_distance(coordDispX, coordDispY, coordDispT,params))
// // 						{
							minVal = minVector[i];
							*correctInd = 0;
// // 						}
						break;
					case 1 :
						coordDispX = (int)dispField->get_value(x,max_int(y-1,0),t,0);
						coordDispY = (int)dispField->get_value(x,max_int(y-1,0),t,1);
						coordDispT = (int)dispField->get_value(x,max_int(y-1,0),t,2);
// // 						if ( check_in_inner_boundaries(arrivalVol,x+coordDispX,y+coordDispY,t+coordDispT,params) && 
// // 							(!check_is_occluded(occVol, x+coordDispX, y+coordDispY, t+coordDispT)) && 
// // 							 check_min_shift_distance(coordDispX, coordDispY, coordDispT,params))
// // 						{
							minVal = minVector[i];
							*correctInd = 1;
// // 						}
						break;
					case 2 :
						coordDispX = (int)dispField->get_value(x,y,max_int(t-1,0),0);
						coordDispY = (int)dispField->get_value(x,y,max_int(t-1,0),1);
						coordDispT = (int)dispField->get_value(x,y,max_int(t-1,0),2);
// // 						if ( check_in_inner_boundaries(arrivalVol,x+coordDispX,y+coordDispY,t+coordDispT,params) && 
// // 							(!check_is_occluded(occVol, x+coordDispX, y+coordDispY, t+coordDispT)) &&
// // 							check_min_shift_distance(coordDispX, coordDispY, coordDispT,params))
// // 						{
							minVal = minVector[i];
							*correctInd = 2;
// 						}
						break;
					default :
						MY_PRINTF("Error in get_min_correct_error. The index i : %d is above 3.\n",i);
				}
			}

		}


	}
	else	//we are looking right, lower, after
	{
        
        dispX = (int)dispField->get_value((int)min_int(x+1,(departVol->xSize)-1),y,t,0); dispY = (int)dispField->get_value((int)min_int(x+1,(departVol->xSize)-1),y,t,1); dispT = (int)dispField->get_value((int)min_int(x+1,(departVol->xSize)-1),y,t,2); 
        if ( check_in_inner_boundaries(arrivalVol,x+dispX,y+dispY,t+dispT,params) && (!check_is_occluded(occVol, x+dispX, y+dispY, t+dispT)) && 
                (!check_already_used_patch( dispField, x, y, t, dispX, dispY, dispT)))
            minVector[0] = ssd_patch_measure(departVol, arrivalVol, dispField,occVol,x,y,t,x+dispX,y+dispY,t+dispT,minError,params);
        
        dispX = (int)dispField->get_value(x,(int)min_int(y+1,(departVol->ySize)-1),t,0); dispY = (int)dispField->get_value(x,(int)min_int(y+1,(departVol->ySize)-1),t,1); dispT = (int)dispField->get_value(x,(int)min_int(y+1,(departVol->ySize)-1),t,2); 
        if ( check_in_inner_boundaries(arrivalVol,x+dispX,y+dispY,t+dispT,params) && (!check_is_occluded(occVol, x+dispX, y+dispY, t+dispT)) && 
                (!check_already_used_patch( dispField, x, y, t, dispX, dispY, dispT)))
            minVector[1] = ssd_patch_measure(departVol, arrivalVol, dispField,occVol,x,y,t,x+dispX,y+dispY,t+dispT,minError,params);
        
        dispX = (int)dispField->get_value(x,y,(int)min_int(t+1,(departVol->tSize)-1),0); dispY = (int)dispField->get_value(x,y,(int)min_int(t+1,(departVol->tSize)-1),1); dispT = (int)dispField->get_value(x,y,(int)min_int(t+1,(departVol->tSize)-1),2); 
        if ( check_in_inner_boundaries(arrivalVol,x+dispX,y+dispY,t+dispT,params) && (!check_is_occluded(occVol, x+dispX, y+dispY, t+dispT)) &&
                (!check_already_used_patch( dispField, x, y, t, dispX, dispY, dispT)))
            minVector[2] = ssd_patch_measure(departVol, arrivalVol,dispField,occVol,x,y,t,x+dispX,y+dispY,t+dispT,minError,params);

		for (i=0;i<NDIMS;i++)
		{
            if (minVector[i] == -1)
                continue;
			if ( minVector[i] < minVal)
			{
				switch (i){
					case 0 :
						coordDispX = (int)dispField->get_value(min_int(x+1,((int)dispField->xSize)-1),y,t,0);
						coordDispY = (int)dispField->get_value(min_int(x+1,((int)dispField->xSize)-1),y,t,1);
						coordDispT = (int)dispField->get_value(min_int(x+1,((int)dispField->xSize)-1),y,t,2);
// // 						if ( check_in_inner_boundaries(arrivalVol,x+coordDispX,y+coordDispY,t+coordDispT,params) && 
// // 							(!check_is_occluded(occVol, x+coordDispX, y+coordDispY, t+coordDispT)) &&
// // 							check_min_shift_distance(coordDispX, coordDispY, coordDispT,params))
// // 						{
							minVal = minVector[i];
							*correctInd = 0;
// 						}
						break;
					case 1 :
						coordDispX = (int)dispField->get_value(x,min_int(y+1,((int)dispField->ySize)-1),t,0);
						coordDispY = (int)dispField->get_value(x,min_int(y+1,((int)dispField->ySize)-1),t,1);
						coordDispT = (int)dispField->get_value(x,min_int(y+1,((int)dispField->ySize)-1),t,2);
// // 						if ( check_in_inner_boundaries(arrivalVol,x+coordDispX,y+coordDispY,t+coordDispT,params) && 
// // 							(!check_is_occluded(occVol, x+coordDispX, y+coordDispY, t+coordDispT)) &&
// // 							check_min_shift_distance(coordDispX, coordDispY, coordDispT,params))
// // 						{
							minVal = minVector[i];
							*correctInd = 1;
// 						}
						break;
					case 2 :
						coordDispX = (int)dispField->get_value(x,y,min_int(t+1,((int)dispField->tSize)-1),0);
						coordDispY = (int)dispField->get_value(x,y,min_int(t+1,((int)dispField->tSize)-1),1);
						coordDispT = (int)dispField->get_value(x,y,min_int(t+1,((int)dispField->tSize)-1),2);
// // 						if ( check_in_inner_boundaries(arrivalVol,x+coordDispX,y+coordDispY,t+coordDispT,params) && 
// // 							(!check_is_occluded(occVol, x+coordDispX, y+coordDispY, t+coordDispT)) &&
// // 							check_min_shift_distance(coordDispX, coordDispY, coordDispT,params))
// // 						{
							minVal = minVector[i];
							*correctInd = 2;
// 						}
						break;
					default :
						MY_PRINTF("Error in get_min_correct_error. The index i : %d is above 3.\n",i);
				}
			}

		}
	}

	if ( (*correctInd) == -1)	//if none of the displacements are valid 
	{
		minVal = -1;
	}
	return(minVal);
}

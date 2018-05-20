import vzinp

video_file = './data/beach_umbrella.avi'
occlusion_file = './data/beach_umbrella_occlusion.png'

if __name__ == '__main__':
    vzinp.start_inpaint(
        video_file=video_file,
        occlusion_file=occlusion_file,
        level='max_level',
        asd=3,
        act='textureFeaturesActivated',
        qwd=1
    )

    print("The inpainting has been finished")

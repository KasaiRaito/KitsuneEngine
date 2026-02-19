# Menu Hover Previews

Place preview media in these folders:

- `assets/previews/angryballs/`
- `assets/previews/dinojump/`

Supported frame extensions: `.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga`, `.webp`
Supported video files: `.mp4`, `.mov`, `.m4v`, `.webm`

If image frames are present, they are used directly.
If no image frames are found and a video file exists, the menu will try to extract frames using `ffmpeg` into `save_data/preview_cache/` and play those.

Frames are sorted by filename and played as a loop while hovering each menu button.
Suggested naming: `frame_0001.png`, `frame_0002.png`, ...

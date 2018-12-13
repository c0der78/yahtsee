package graphics

import (
	"github.com/ryjen/imgui-go"
	"image"
)

type Graphics interface {
	Update()
	NewFrame()
	Render(data imgui.DrawData)
	Start(width int, height int, title string) error
	Stop()
	Showing() bool
	LoadImage(rgba *image.RGBA) (imgui.TextureID, error)
}

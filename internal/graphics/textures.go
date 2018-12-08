package graphics

import (
	"github.com/raedatoui/glutils"
	"github.com/ryjen/imgui-go"
	"image"
	"image/jpeg"
	"image/png"
)

type TextureTypes uint

const (
	Die1 TextureTypes = iota
	Die2
	Die3
	Die4
	Die5
	Die6
	Last
	First = Die1
)

var texturePaths = []string{
	"assets/Die1.png",
	"assets/Die2.png",
	"assets/Die3.png",
	"assets/Die4.png",
	"assets/Die5.png",
	"assets/Die6.png",
}

type Texture struct {
	Id imgui.TextureID
	Width int
	Height int
}

type TextureMap struct {
	values map[TextureTypes]*Texture
}

var Textures = &TextureMap{
	values: make(map[TextureTypes]*Texture),
}

func init() {
	image.RegisterFormat("png", "png", png.Decode, png.DecodeConfig)
	image.RegisterFormat("jpeg", "jpeg", jpeg.Decode, jpeg.DecodeConfig)
}

func (textureMap *TextureMap) Load(impl Graphics) error {

	for asset := First; asset < Last; asset++ {

		texture := &Texture{}

		rgba, err := glutils.ImageToPixelData(texturePaths[asset])

		if err != nil {
			return err
		}

		texture.Width = rgba.Rect.Size().X
		texture.Height = rgba.Rect.Size().Y

		texture.Id, err = impl.LoadImage(rgba)

		if err != nil {
			return err
		}

		textureMap.values[asset] = texture
	}
	return nil
}

func (textureMap *TextureMap) Get(texture TextureTypes) *Texture {
	return textureMap.values[texture]
}


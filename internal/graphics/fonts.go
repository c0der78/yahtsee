package graphics

import "github.com/ryjen/imgui-go"

type FontTypes = int

const (
	FontDefault FontTypes = iota
	FontAwesome
	FontAwesomeLarge
)

const (
	FontAwesomeDie1 = '\uF525'
	FontAwesomeDie2 = '\uF528'
	FontAwesomeDie3 = '\uF527'
	FontAwesomeDie4 = '\uF524'
	FontAwesomeDie5 = '\uF523'
	FontAwesomeDie6 = '\uF526'
)

type FontMap struct {
	values map[FontTypes]imgui.Font
}

var Fonts = &FontMap {
	values: make(map[FontTypes]imgui.Font),
}

func (fonts *FontMap) Get(font FontTypes) imgui.Font {
	return fonts.values[font]
}

func (fonts *FontMap) Load() {

	atlas := imgui.CurrentIO().Fonts()

	font := atlas.AddFontDefault()

	fonts.values[FontDefault] = font

	font = atlas.AddFontFromFileTTF("assets/fontawesome.otf", 13)

	fonts.values[FontAwesome] = font

	font = atlas.AddFontFromFileTTF("assets/fontawesome.otf", 22)

	fonts.values[FontAwesomeLarge] = font
}

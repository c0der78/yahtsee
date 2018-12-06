package internal

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/views"
	"time"
)

type Ui struct {
	graphics *Graphics
	context *imgui.Context
	views []View
}

func NewUi() *Ui {
	return &Ui{}
}

func (ui *Ui) Add(view View) {
	ui.views = append(ui.views, view)
}

func (ui *Ui) Init(config *Config) error {
	if ui.graphics != nil {
		return nil
	}

	ui.context = imgui.CreateContext(nil)

	window, err := NewWindow(config.Defaults.Width, config.Defaults.Height, config.Title)

	if err != nil {
		return err
	}

	ui.graphics = NewGraphics(window)

	menu := &views.MenuCallbacks{
		OnGameExit: func() {
			config.Over = true
		},
		OnGameNew: func() {
			ui.Add(views.NewNewGameView())
		},
	}

	ui.Add(views.NewMenuView(menu))

	ui.Add(views.NewDemoView())

	ui.Add(views.NewBoardView(ui.graphics.textures.Dice))

	return nil
}

func (ui *Ui) Close() {
	ui.graphics.Close()
	ui.context.Destroy()
}

func (ui *Ui) IsOpen() bool {
	return !ui.graphics.window.ShouldClose()
}

func (ui *Ui) Update() {
	ui.graphics.Update()

	for _, view := range ui.views {
		view.Update()
	}
}

func (ui *Ui) Render() {
	ui.graphics.NewFrame()

	for _, view := range ui.views {
		view.Render()
	}

	ui.graphics.Clear()

	imgui.Render()

	ui.graphics.Render(imgui.RenderedDrawData())

	ui.graphics.window.SwapBuffers()
	<- time.After(time.Millisecond * 25)
}

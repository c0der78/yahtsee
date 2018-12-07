package internal

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/graphics"
	"micrantha.com/yahtsee/internal/views"
)

type Graphics interface {
	Update()
	NewFrame()
	Render(data imgui.DrawData)
	Start(width int, height int, title string) error
	Stop()
	Showing() bool
}

type Ui struct {
	graphics Graphics
	context *imgui.Context
	views []View
}

func NewUi() *Ui {
	return &Ui{
		graphics: graphics.NewSDL(),
		context: imgui.CreateContext(nil),
	}
}

func (ui *Ui) Add(view View) {
	ui.views = append(ui.views, view)
}

func (ui *Ui) Init(config *Config) error {

	err := ui.graphics.Start(config.Defaults.Width, config.Defaults.Height, config.Title)

	if err != nil {
		return err
	}

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

	ui.Add(views.NewBoardView())

	return nil
}

func (ui *Ui) Close() {
	ui.graphics.Stop()
	ui.context.Destroy()
}

func (ui *Ui) IsOpen() bool {
	return ui.graphics.Showing()
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

	imgui.Render()

	ui.graphics.Render(imgui.RenderedDrawData())
}

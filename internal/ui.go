package internal

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/graphics"
	"micrantha.com/yahtsee/internal/views"
)

const MainWindowID = "MainWindow"

type Ui struct {
	graphics graphics.Graphics
	context  *imgui.Context
	views    []views.View
}

func NewUi() *Ui {
	return &Ui{
		graphics: graphics.NewGLFW(),
		context:  imgui.CreateContext(nil),
	}
}

func (ui *Ui) Add(view views.View) {
	ui.views = append(ui.views, view)
}

func (ui *Ui) loadViews(config *Config)  {

	menu := &views.MenuCallbacks{
		OnGameExit: func() {
			config.Over = true
		},
		OnGameNew: func() {
			ui.Add(views.NewNewGameView())
		},
	}

	ui.Add(views.NewMenuView(menu))

	ui.Add(views.NewSheetView())

	ui.Add(views.NewSideView())
}

func (ui *Ui) Init(config *Config) error {

	graphics.Fonts.Load()

	err := ui.graphics.Start(config.Defaults.Width, config.Defaults.Height, config.Title)

	if err != nil {
		return err
	}

	err = graphics.Textures.Load(ui.graphics)

	if err != nil {
		return err
	}


	ui.loadViews(config)

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

	if imgui.BeginV(MainWindowID, nil,
		imgui.WindowFlagsAlwaysAutoResize|imgui.WindowFlagsNoTitleBar|
			imgui.WindowFlagsNoCollapse|imgui.WindowFlagsNoMove) {

		imgui.PushFont(graphics.Fonts.Get(graphics.FontAwesome))

		for _, view := range ui.views {
			view.Render()
		}

		imgui.PopFont()
	}

	imgui.End()

	imgui.Render()

	ui.graphics.Render(imgui.RenderedDrawData())
}

package internal

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/graphics"
	"micrantha.com/yahtsee/internal/views"
)

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

func (ui *Ui) loadViews(config *Config) {

	menu := &views.MenuCallbacks{
		OnGameExit: func() {
			config.Over = true
		},
		OnGameNew: func() {
			ui.Add(views.NewNewGameView())
		},
	}

	shaker := views.NewDiceView()

	ui.Add(views.NewMenuView(menu))
	ui.Add(views.NewPlayersView())
	ui.Add(shaker)
	ui.Add(views.NewChatView())
	ui.Add(views.NewSheetView(shaker))
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

	imgui.PushFont(graphics.Fonts.Get(graphics.FontAwesome))

	for _, view := range ui.views {
		view.Render()
	}

	imgui.PopFont()

	imgui.Render()

	ui.graphics.Render(imgui.RenderedDrawData())
}

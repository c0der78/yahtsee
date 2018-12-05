package internal

import (
	"fmt"
	"github.com/inkyblackness/imgui-go"
	"time"
)

type Ui struct {
	graphics *Graphics
	context *imgui.Context
	counter int
	showDemo bool
}

func NewUi() *Ui {
	return &Ui{}
}

func (ui *Ui) Init() error {
	if ui.graphics != nil {
		return nil
	}

	ui.context = imgui.CreateContext(nil)

	window, err := NewWindow(1280, 720, "Yahtsee")

	if err != nil {
		return err
	}

	ui.graphics = NewGraphics(window)

	return nil
}

func (ui *Ui) Close() {
	ui.context.Destroy()
	ui.graphics.Close()
}

func (ui *Ui) IsOpen() bool {
	return !ui.graphics.window.ShouldClose()
}

func (ui *Ui) Update() {
	ui.graphics.Update()
}

func (ui *Ui) Render() {
	ui.graphics.NewFrame()


	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		imgui.Text("Hello, world!")

		imgui.Checkbox("Demo Window", &ui.showDemo)

		if imgui.Button("Button") {
			ui.counter++
		}
		imgui.SameLine()
		imgui.Text(fmt.Sprintf("counter = %d", ui.counter))

	}

	// 3. Show the ImGui demo window. Most of the sample code is in imgui.ShowDemoWindow().
	// Read its code to learn more about Dear ImGui!
	if ui.showDemo {
		imgui.ShowDemoWindow(&ui.showDemo)
	}

	ui.graphics.Clear()

	imgui.Render()

	ui.graphics.Render(imgui.RenderedDrawData())

	ui.graphics.window.SwapBuffers()
	<- time.After(time.Millisecond * 25)
}
package views

import "github.com/ryjen/imgui-go"

type MenuCallbacks struct {
	OnGameNew func()
	OnGameExit func()
}

type MenuView struct {
	callbacks *MenuCallbacks
	showAbout bool
}

func NewMenuView(callbacks *MenuCallbacks) *MenuView {
	return &MenuView{
		callbacks: callbacks,
		showAbout:	false,
	}
}

func (view *MenuView) Render() {

	if !imgui.BeginMainMenuBar() {
		return
	}

	if imgui.BeginMenu("Game") {

		if imgui.MenuItemV("New", "N", false, true) {
			view.callbacks.OnGameNew()
		}

		if imgui.MenuItemV("Exit", "X", false, true) {
			view.callbacks.OnGameExit()
		}

		imgui.EndMenu()
	}

	if imgui.BeginMenu("Help") {
		if imgui.MenuItemV("About", "A", false, true) {
			view.showAbout = true
		}

		imgui.EndMenu()
	}

	imgui.EndMainMenuBar()

	if view.showAbout {
		view.About()
	}
}

func (view *MenuView) About() {
	if !imgui.Begin("About") {
		return
	}

	imgui.Text("Yahtsee v0.1.0")

	imgui.End()
}

func (view *MenuView) Update() {}
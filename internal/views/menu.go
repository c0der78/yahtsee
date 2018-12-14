package views

import "github.com/ryjen/imgui-go"

type MenuCallbacks struct {
	OnGameNew  func()
	OnGameExit func()
}

type MenuView struct {
	callbacks *MenuCallbacks
	showAbout bool
	showDemo  bool
}

func NewMenuView(callbacks *MenuCallbacks) *MenuView {
	return &MenuView{
		callbacks: callbacks,
		showAbout: false,
		showDemo:  false,
	}
}

func (view *MenuView) Render() {

	if imgui.BeginMainMenuBar() {

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

			if imgui.MenuItemV("Demo", "D", false, true) {
				view.showDemo = true
			}
			imgui.EndMenu()
		}
	}

	imgui.EndMainMenuBar()

	if view.showAbout {
		view.About()
	}

	if view.showDemo {
		imgui.ShowDemoWindow(&view.showDemo)
	}
}

func (view *MenuView) About() {
	if imgui.BeginPopupModal("About") {

		imgui.Spacing()
		imgui.Text("Yahtsee v0.1.0")
		imgui.Spacing()

		if imgui.Button("OK") {
			view.showAbout = false
			imgui.CloseCurrentPopup()
		}

		imgui.EndPopup()
	}

	imgui.OpenPopup("About")
}

func (view *MenuView) Update() {}

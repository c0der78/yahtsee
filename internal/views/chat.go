package views

import "github.com/ryjen/imgui-go"

const chatViewID = "ChatView"

var chatViewSize = imgui.Vec2{X: 200, Y: 110 }

type ChatView struct {}

func NewChatView() *ChatView {
	return &ChatView{}
}

func (view *ChatView) Render() {

	if imgui.BeginChildV(chatViewID, chatViewSize, true, imgui.WindowFlagsMenuBar) {

		if imgui.BeginMenuBar() {

			if imgui.BeginMenu("Chat") {
				imgui.EndMenu()
			}
			imgui.EndMenuBar()
		}

	}

	imgui.EndChild()
}

func (view *ChatView) Update() {

}

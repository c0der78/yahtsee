package views

import "github.com/ryjen/imgui-go"

const chatViewID = "Chat"

var chatViewSize = imgui.Vec2{X: 200, Y: 110}

var chatViewPos = imgui.Vec2{X: 700, Y: 400}

type ChatView struct{}

func NewChatView() *ChatView {
	return &ChatView{}
}

func (view *ChatView) Render() {

	imgui.SetNextWindowPos(chatViewPos)
	imgui.SetNextWindowSize(chatViewSize)

	if imgui.Begin(chatViewID) {

	}

	imgui.End()
}

func (view *ChatView) Update() {

}

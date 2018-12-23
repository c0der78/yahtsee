package views

import "github.com/ryjen/imgui-go"

const chatViewID = "Chat"

var chatViewSize = imgui.Vec2{X: 200, Y: 160}

var chatViewPos = imgui.Vec2{X: 700, Y: 360}

const chatTextID = "ChatText"

var chatTextSize = imgui.Vec2{X: 185, Y: 30}

const chatMessagesID = "ChatMessages"

var chatMessagesSize = imgui.Vec2{X:200, Y:85}

type ChatView struct{}

func NewChatView() *ChatView {
	return &ChatView{}
}

func (view *ChatView) Render() {

	imgui.SetNextWindowPos(chatViewPos)
	imgui.SetNextWindowSize(chatViewSize)

	if imgui.Begin(chatViewID) {

		if imgui.BeginChildV(chatMessagesID, chatMessagesSize, false, 0) {

		}
		imgui.EndChild()

		if imgui.BeginChildV(chatTextID, chatTextSize, false, 0) {
		}
		imgui.EndChild()
	}

	imgui.End()
}

func (view *ChatView) Update() {

}

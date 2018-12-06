package internal

type View interface {
	Render()
	Update()
}

const SideViewId = "SideView"
const PlayersViewId = "PlayersView"
const DiceViewId = "DiceView"
const ChatViewId = "ChatView"


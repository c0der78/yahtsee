package internal

import "runtime"

type Game struct {
	ui *Ui
}

func NewGame() *Game {
	return &Game{
		NewUi(),
	}
}

func (game *Game) Start() error {

	runtime.LockOSThread()

	return game.ui.Init()
}

func (game *Game) IsOver() bool {
	return !game.ui.IsOpen()
}

func (game *Game) Update() {
	game.ui.Update()
}

func (game *Game) Render() {
	game.ui.Render()
}

func (game *Game) Finish() {
	game.ui.Close()
}

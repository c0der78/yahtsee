package main

import (
	"micrantha.com/yahtsee/internal"
)

func main() {

	game := internal.NewGame()

	game.Start()

	for !game.IsOver() {
		game.Update()

		game.Render()
	}

	game.Finish()
}
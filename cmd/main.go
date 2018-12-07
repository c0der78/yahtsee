package main

import (
	"log"
	"micrantha.com/yahtsee/internal"
)

func main() {

	game := internal.NewGame()

	err := game.Start()

	if err != nil {
		log.Print(err)
	}

	for !game.IsOver() {

		game.Update()

		game.Render()
	}

	game.Finish()
}
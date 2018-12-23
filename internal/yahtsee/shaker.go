package yahtsee

import (
	"math/rand"
)

const DiePerRoll = 5

type Roll = [DiePerRoll]int

func Shake() Roll {

	dice := Roll{}

	for i := 0; i < len(dice); i++ {
		die := rand.Int()%6 + 1

		dice[i] = die
	}

	return dice
}

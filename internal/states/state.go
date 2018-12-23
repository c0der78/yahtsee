package states

import "micrantha.com/yahtsee/internal/yahtsee"

type State struct {
	CurrentRoll yahtsee.Roll
	Turn int
}

type Stateful interface {
	State() *State
}

func New() *State {
	return &State{
		CurrentRoll:yahtsee.Shake(),
		Turn: -1,
	}
}
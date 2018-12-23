package events

import "micrantha.com/yahtsee/internal/yahtsee"

const ScoreEventID = "score"

type ScoreEvent struct {
	Value yahtsee.ScoreTypes
}

func NewScoreEvent(value yahtsee.ScoreTypes) *ScoreEvent{
	return &ScoreEvent{ Value: value }
}

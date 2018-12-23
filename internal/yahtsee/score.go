package yahtsee

type ScoreValue = int

type ScoreTypes int

const (
	Aces ScoreTypes = iota
	Twos
	Threes
	Fours
	Fives
	Sixes
	Kind3
	Kind4
	FullHouse
	StraightSm
	StrightLg
	Yahtsee
	Chance
	Max
)

var ScoreNames = [Max]string{
	"Aces = 1",
	"Twos = 2",
	"Threes = 3",
	"Fours = 4",
	"Fives = 5",
	"Sixes = 6",
	"3 of a Kind",
	"4 of a Kind",
	"Full House",
	"Small Straight (4)",
	"Large Straight (5)",
	"Yahtsee",
	"Chance",
}

var ScoreHelp = [Max]string{
	"Count and add only aces",
	"Count and add only twos",
	"Count and add only threes",
	"Count and add only fours",
	"Count and add only fives",
	"Count and add only sixes",
	"Add total of all dice",
	"Add total of all dice",
	"Score 25",
	"Score 30",
	"Score 40",
	"Score 50",
	"Score total of all dice",
}

// Score A representation of the current score
type Score struct {
	values [Max]ScoreValue
}

func NewScore() *Score {
	return &Score {
		values: [Max]ScoreValue{
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		},
	}
}

func countRoll(roll Roll, value ScoreValue) int {
	count := 0
	for _, val := range roll {
		if val == value {
			count += value
		}
	}
	return count
}

func totalRoll(roll Roll) int {
	count := 0
	for _, val := range roll {
		count += val
	}
	return count
}

// SetScore sets a score value
func (score *Score) Set(position ScoreTypes, roll Roll) {
	switch position {
	case Aces:
		fallthrough
	case Twos:
		fallthrough
	case Threes:
		fallthrough
	case Fours:
		fallthrough
	case Fives:
		fallthrough
	case Sixes:
		score.values[position] = countRoll(roll, int(position)+1)
		break
	case Kind3:
		fallthrough
	case Kind4:
		fallthrough
	case Chance:
		score.values[position] = totalRoll(roll)
		break
	case FullHouse:
		score.values[position] = 25
		break
	case StraightSm:
		score.values[position] = 30
		break
	case StrightLg:
		score.values[position] = 40
		break
	case Yahtsee:
		score.values[position] = 50
		break
	default:
		panic("invalid score type")
	}
}

func (score *Score) Get(position ScoreTypes) int {
	return score.values[position]
}

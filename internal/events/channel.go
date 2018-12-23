package events

type Channel struct {
	value chan Event
	callback func(event Event)
	listening bool
}

func NewChannel(callback func (event Event) ) *Channel {
	return &Channel{
		value: make(chan Event),
		callback: callback,
		listening: false,
	}
}

func (ch *Channel) StartListening() {
	go func () {
		for {
			ev := <-ch.value
			ch.callback(ev)
		}
	}()
}

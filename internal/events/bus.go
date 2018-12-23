package events

type eventBus struct {
	listeners map[string][]*Channel
}
var instance = &eventBus{}

func Bus() *eventBus {
	return instance
}

func (e *eventBus) AddListener(id string, ch *Channel) {
	if e.listeners == nil {
		e.listeners = make(map[string][]*Channel)
	}

	if _, ok := e.listeners[id]; ok {
		e.listeners[id] = append(e.listeners[id] , ch)
	} else {
		e.listeners[id] = []*Channel{ch}
	}
}

func (e *eventBus) RemoveListener(id string, ch *Channel) {
	if _, ok := e.listeners[id]; !ok {
		return
	}
	for i := range e.listeners[id] {
		if e.listeners[id][i] == ch {
			e.listeners[id] = append(e.listeners[id][:i],
				e.listeners[id][i+1:]...)
			break
		}
	}
}

func (e *eventBus) Emit(id string, response Event) {
	if _, ok := e.listeners[id]; !ok {
		return
	}
	for _, handler := range e.listeners[id] {
		go func(handler *Channel) {
			handler.value <- response
		}(handler)
	}
}

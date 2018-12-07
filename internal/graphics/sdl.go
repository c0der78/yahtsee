package graphics

import (
	"errors"
	"github.com/banthar/Go-SDL/gfx"
	"github.com/banthar/Go-SDL/sdl"
	"github.com/ryjen/imgui-go"
	"time"
)

const (
	BPP = 32
	DEPTH = 24
	STENCIL = 8
)

type SDL struct {
	opengl
	window *sdl.Surface
	fps *gfx.FPSmanager
	time uint32
	showing bool
}

func NewSDL() *SDL {
	return &SDL{
		opengl: openGL(),
		fps: gfx.NewFramerate(),
	}
}

func (impl *SDL) Start(width int, height int, title string) error {

	errno := sdl.Init(sdl.INIT_VIDEO|sdl.INIT_TIMER)

	if errno != 0 {
		return errors.New(sdl.GetError())
	}

	sdl.GL_SetAttribute(sdl.GL_DOUBLEBUFFER,1)
	sdl.GL_SetAttribute(sdl.GL_DEPTH_SIZE, DEPTH)
	sdl.GL_SetAttribute(sdl.GL_STENCIL_SIZE, STENCIL)

	io := imgui.CurrentIO()

	io.KeyMap(imgui.KeyTab, sdl.K_TAB)
	io.KeyMap(imgui.KeyLeftArrow, sdl.K_LEFT)
	io.KeyMap(imgui.KeyRightArrow, sdl.K_RIGHT)
	io.KeyMap(imgui.KeyUpArrow, sdl.K_UP)
	io.KeyMap(imgui.KeyDownArrow, sdl.K_DOWN)
	io.KeyMap(imgui.KeyPageUp, sdl.K_PAGEUP)
	io.KeyMap(imgui.KeyPageDown, sdl.K_PAGEDOWN)
	io.KeyMap(imgui.KeyHome, sdl.K_HOME)
	io.KeyMap(imgui.KeyEnd, sdl.K_END)
	io.KeyMap(imgui.KeyInsert, sdl.K_INSERT)
	io.KeyMap(imgui.KeyDelete, sdl.K_DELETE)
	io.KeyMap(imgui.KeyBackspace, sdl.K_BACKSPACE)
	io.KeyMap(imgui.KeySpace, sdl.K_SPACE)
	io.KeyMap(imgui.KeyEnter, sdl.K_KP_ENTER)
	io.KeyMap(imgui.KeyEscape, sdl.K_ESCAPE)
	io.KeyMap(imgui.KeyA, sdl.K_a)
	io.KeyMap(imgui.KeyC, sdl.K_c)
	io.KeyMap(imgui.KeyV, sdl.K_v)
	io.KeyMap(imgui.KeyX, sdl.K_x)
	io.KeyMap(imgui.KeyY, sdl.K_y)
	io.KeyMap(imgui.KeyZ, sdl.K_z)

	impl.window = sdl.SetVideoMode(width, height, BPP, sdl.OPENGL|sdl.HWSURFACE|sdl.GL_DOUBLEBUFFER)

	if impl.window == nil {
		return errors.New(sdl.GetError())
	}

	sdl.WM_SetCaption(title, "")

	sdl.EnableUNICODE(1)

	err := impl.startGL(width, height)

	if err != nil {
		return err
	}

	impl.showing = true

	return nil
}

func (impl *SDL) NewFrame() {

	io := imgui.CurrentIO()

	io.SetDisplaySize(imgui.Vec2{
		X: float32(impl.window.W),
		Y: float32(impl.window.H),
	})

	// Setup time step
	currentTime := sdl.GetTicks()
	//io.SetDeltaTime(float32(currentTime - impl.time) / 1000)
	impl.time = currentTime

	imgui.NewFrame()
}

func (impl *SDL) Stop() {
	impl.showing = false
	sdl.Quit()
	impl.stopGL()
}

func (impl *SDL) Render(data imgui.DrawData) {

	impl.clearGL()

	//impl.window.FillRect(nil, 0x000000)

	displayWidth, displayHeight := impl.window.W, impl.window.H
	display := imgui.Vec2{X: float32(displayWidth), Y: float32(displayHeight) }

	fbWidth, fbHeight := impl.window.Clip_rect.W, impl.window.Clip_rect.H
	fb := imgui.Vec2{X: float32(fbWidth), Y: float32(fbHeight) }

	impl.renderGL(data, display, fb)

	sdl.GL_SwapBuffers()
	sdl.Delay(uint32(time.Millisecond * 25))
}

func (impl *SDL) Update() {
	ev := sdl.PollEvent();
	switch e := ev.(type) {
	case *sdl.QuitEvent:
		impl.showing = false
		break
	case *sdl.KeyboardEvent:
		if e.Keysym.Sym == 27 {
			impl.showing = false
		}
		break

	case *sdl.MouseButtonEvent:
		break

	case *sdl.ResizeEvent:
		impl.window = sdl.SetVideoMode(int(e.W), int(e.H), BPP, sdl.RESIZABLE)

		if impl.window == nil {
			panic(sdl.GetError())
		}
	}
}

func (impl *SDL) Showing() bool {
	return impl.showing
}
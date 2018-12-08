package graphics

import (
	"github.com/go-gl/gl/v4.1-core/gl"
	"github.com/go-gl/glfw/v3.2/glfw"
	"github.com/ryjen/imgui-go"
	"image"
	"math"
	"time"
)

type Window = glfw.Window

type GLFW struct {
	opengl
	window           *Window
	time             float64
	mouseJustPressed [3]bool
}

func newWindow(width int, height int, title string) (*Window, error) {

	err := glfw.Init()

	if err != nil {
		return nil, err
	}

	glfw.WindowHint(glfw.ContextVersionMajor, 3)
	glfw.WindowHint(glfw.ContextVersionMinor, 2)
	glfw.WindowHint(glfw.OpenGLProfile, glfw.OpenGLCoreProfile)
	glfw.WindowHint(glfw.OpenGLForwardCompatible, 1)

	window, err := glfw.CreateWindow(width, height, title, nil, nil)

	if err != nil {
		return window, err
	}

	window.MakeContextCurrent()

	glfw.SwapInterval(1)

	return window, err
}

func (impl *GLFW) Stop() {
	impl.window.Destroy()
	glfw.Terminate()
	impl.stopGL()
}

func (impl *GLFW) Update() {
	glfw.PollEvents()
}

func (impl *GLFW) Showing() bool {
	return !impl.window.ShouldClose()
}

func NewGLFW() *GLFW {
	return &GLFW {
		opengl: openGL(),
	}
}

func (impl *GLFW) Start(width int, height int, title string) error {

	window, err := newWindow(width, height, title)
	
	if err != nil {
		return err
	}

	impl.window = window

	err = impl.startGL(width, height)

	if err != nil {
		return err
	}

	io := imgui.CurrentIO()

	io.KeyMap(imgui.KeyTab, int(glfw.KeyTab))
	io.KeyMap(imgui.KeyLeftArrow, int(glfw.KeyLeft))
	io.KeyMap(imgui.KeyRightArrow, int(glfw.KeyRight))
	io.KeyMap(imgui.KeyUpArrow, int(glfw.KeyUp))
	io.KeyMap(imgui.KeyDownArrow, int(glfw.KeyDown))
	io.KeyMap(imgui.KeyPageUp, int(glfw.KeyPageUp))
	io.KeyMap(imgui.KeyPageDown, int(glfw.KeyPageDown))
	io.KeyMap(imgui.KeyHome, int(glfw.KeyHome))
	io.KeyMap(imgui.KeyEnd, int(glfw.KeyEnd))
	io.KeyMap(imgui.KeyInsert, int(glfw.KeyInsert))
	io.KeyMap(imgui.KeyDelete, int(glfw.KeyDelete))
	io.KeyMap(imgui.KeyBackspace, int(glfw.KeyBackspace))
	io.KeyMap(imgui.KeySpace, int(glfw.KeySpace))
	io.KeyMap(imgui.KeyEnter, int(glfw.KeyEnter))
	io.KeyMap(imgui.KeyEscape, int(glfw.KeyEscape))
	io.KeyMap(imgui.KeyA, int(glfw.KeyA))
	io.KeyMap(imgui.KeyC, int(glfw.KeyC))
	io.KeyMap(imgui.KeyV, int(glfw.KeyV))
	io.KeyMap(imgui.KeyX, int(glfw.KeyX))
	io.KeyMap(imgui.KeyY, int(glfw.KeyY))
	io.KeyMap(imgui.KeyZ, int(glfw.KeyZ))

	impl.installCallbacks()

	return nil
}

func (impl *GLFW) NewFrame() {

	// Setup display size (every frame to accommodate for window resizing)
	windowWidth, windowHeight := impl.window.GetSize()

	io := imgui.CurrentIO()
	io.SetDisplaySize(imgui.Vec2{X: float32(windowWidth), Y: float32(windowHeight)})

	// Setup time step
	currentTime := glfw.GetTime()
	if impl.time > 0 {
		io.SetDeltaTime(float32(currentTime - impl.time))
	}
	impl.time = currentTime

	// Setup inputs
	if impl.window.GetAttrib(glfw.Focused) != 0 {
		x, y := impl.window.GetCursorPos()
		io.SetMousePosition(imgui.Vec2{X: float32(x), Y: float32(y)})
	} else {
		io.SetMousePosition(imgui.Vec2{X: -math.MaxFloat32, Y: -math.MaxFloat32})
	}

	for i := 0; i < len(impl.mouseJustPressed); i++ {
		down := impl.mouseJustPressed[i] || (impl.window.GetMouseButton(buttonIDByIndex[i]) == glfw.Press)
		io.SetMouseButtonDown(i, down)
		impl.mouseJustPressed[i] = false
	}

	imgui.NewFrame()
}

func (impl *GLFW) Render(data imgui.DrawData) {

	impl.clearGL()

	displayWidth, displayHeight := impl.window.GetSize()
	display := imgui.Vec2{X: float32(displayWidth), Y: float32(displayHeight) }

	fbWidth, fbHeight := impl.window.GetFramebufferSize()
	fb := imgui.Vec2{X: float32(fbWidth), Y: float32(fbHeight) }

	impl.renderGL(data, display, fb)

	impl.window.SwapBuffers()
	<- time.After(time.Millisecond * 25)
}


func (impl *GLFW) installCallbacks() {
	impl.window.SetMouseButtonCallback(impl.mouseButtonChange)
	impl.window.SetScrollCallback(impl.mouseScrollChange)
	impl.window.SetKeyCallback(impl.keyChange)
	impl.window.SetCharCallback(impl.charChange)
}

var buttonIndexByID = map[glfw.MouseButton]int{
	glfw.MouseButton1: 0,
	glfw.MouseButton2: 1,
	glfw.MouseButton3: 2,
}

var buttonIDByIndex = map[int]glfw.MouseButton{
	0: glfw.MouseButton1,
	1: glfw.MouseButton2,
	2: glfw.MouseButton3,
}

func (impl *GLFW) mouseButtonChange(window *glfw.Window, rawButton glfw.MouseButton, action glfw.Action, mods glfw.ModifierKey) {
	buttonIndex, known := buttonIndexByID[rawButton]

	if known && (action == glfw.Press) {
		impl.mouseJustPressed[buttonIndex] = true
	}
}

func (impl *GLFW) mouseScrollChange(window *glfw.Window, x, y float64) {
	io := imgui.CurrentIO()
	io.AddMouseWheelDelta(float32(x), float32(y))
}

func (impl *GLFW) keyChange(window *glfw.Window, key glfw.Key, scancode int, action glfw.Action, mods glfw.ModifierKey) {
	io := imgui.CurrentIO()
	if action == glfw.Press {
		io.KeyPress(int(key))
	}
	if action == glfw.Release {
		io.KeyRelease(int(key))
	}

	// Modifiers are not reliable across systems
	io.KeyCtrl(int(glfw.KeyLeftControl), int(glfw.KeyRightControl))
	io.KeyShift(int(glfw.KeyLeftShift), int(glfw.KeyRightShift))
	io.KeyAlt(int(glfw.KeyLeftAlt), int(glfw.KeyRightAlt))
	io.KeySuper(int(glfw.KeyLeftSuper), int(glfw.KeyRightSuper))
}

func (impl *GLFW) charChange(window *glfw.Window, char rune) {
	io := imgui.CurrentIO()
	io.AddInputCharacters(string(char))
}

func (impl *GLFW) LoadImage(rgba *image.RGBA) (imgui.TextureID, error) {
	var textureID uint32

	gl.GenTextures(1, &textureID)

	gl.BindTexture(gl.TEXTURE_2D, textureID)

	pixels := gl.Ptr(rgba.Pix)

	size := rgba.Rect.Size()

	gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, int32(size.X), int32(size.Y),
		0, gl.RGBA, gl.UNSIGNED_BYTE, pixels)

	gl.GenerateMipmap(gl.TEXTURE_2D)

	gl.BindTexture(gl.TEXTURE_2D, 0)

	return imgui.TextureID(textureID), nil
}

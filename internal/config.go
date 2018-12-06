package internal

import (
	"encoding/json"
	"os"
	"os/user"
	"path"
	"strings"
)

type Defaults struct {
	Height int
	Width int
}

type Config struct {
	Title string `json:title`
	Defaults Defaults `json:defaults`
	Over bool `json:ignore`
}

func NewConfig() *Config {
	return &Config{
		Title: strings.ToTitle(os.Args[0]),
		Defaults: Defaults{
			Width: 1280,
			Height: 720,
		},
		Over: false,
	}
}

func (conf *Config) Load() error {
	u, err := user.Current()

	if err != nil {
		return err
	}

	fileName := path.Join(u.HomeDir, ".config", "yahtsee.json")

	file, err := os.Open(fileName)

	if err != nil {
		return err
	}

	defer file.Close()

	decoder := json.NewDecoder(file)

	return decoder.Decode(conf)
}
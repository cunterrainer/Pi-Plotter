package main

import (
	"C"
	"fmt"
	"io"
	"net/http"
	"os"
    "github.com/schollz/progressbar/v3"
)

func ErrorStr(msg string, err error) string {
	return fmt.Sprintf("%s [%s]", msg, err)
}

//export Download
func Download(url string, path string) string {
	req, err := http.NewRequest("GET", url, nil)
	if err != nil {
		return ErrorStr("Failed to create http request", err)
	}
	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return ErrorStr("Failed to get response", err)
	}
	defer resp.Body.Close()

	const tempPath string = ".tmp"
	f, err := os.OpenFile(tempPath, os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		return ErrorStr("Failed to open temporary file ["+tempPath+"]", err)
	}
	defer f.Close()

    fmt.Printf("Downloading '%s' from: '%s'\n", path, url)
    bar := progressbar.DefaultBytes(resp.ContentLength, "Downloading")
    w, err := io.Copy(io.MultiWriter(f, bar), resp.Body)
    if err != nil {
        return ErrorStr(fmt.Sprintf("Error while downloading [%d/%d]", w, resp.Body), err)
    }
    println("Finished downloading")
    
    f.Close()
	e := os.Rename(tempPath, path)
	if e != nil {
		return ErrorStr(fmt.Sprintf("Failed to rename temporary file '%s' to '%s'", tempPath, path), e)
	}
    fmt.Printf("Renamed '%s' to '%s'\n", tempPath, path)
	return ""
}

func main() {}

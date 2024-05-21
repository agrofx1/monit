package main

import (
	"encoding/base64"
	"fmt"
	"math"
	"os"
	"strconv"
	"time"

	"net/http"

	"github.com/pbnjay/memory"
	"github.com/shirou/gopsutil/cpu"
	"github.com/shirou/gopsutil/host"
	"github.com/ssimunic/gosensors"
)

var URI string = os.Args[1];
var CODE string = os.Args[2];

var CHIP string = os.Args[3];
var VAL string = os.Args[4];

var numbers = map[string]bool {
    "0": true,
    "1": true,
	"2": true,
	"3": true,
	"4": true,
	"5": true,
	"6": true,
	"7": true,
	"8": true,
	"9": true,
}

func main() {
	fmt.Println("Starting...");
	
	base64.RawStdEncoding.WithPadding(base64.NoPadding)

	for range time.Tick(time.Second * 2) {
		bytes := []byte(getStats())
		var base string = base64.RawURLEncoding.EncodeToString(bytes);
		sendData(base);
	}
}

func getStats() string {
	var result string;
	percent,cpu_err := cpu.Percent(time.Second,false);
	system,system_err := host.Info();
	sensors,sensors_err := gosensors.NewFromSystem()
	ram := int(memory.TotalMemory() / 1000000000);
	ram_free := int(memory.FreeMemory() / 1000000000);
	ram_usage := int(ram - ram_free);
	hostname := system.Hostname;
	var temp string;
	if (cpu_err != nil || system_err != nil || sensors_err != nil) {
		return "Error"
	}
	for chip := range sensors.Chips {
		if (chip == CHIP) {
			for key, value := range sensors.Chips[chip] {
				if key == VAL {
					temp = value;
				}
			}
		}

	}
	if (temp == "") {
		temp = " "
	} else {
		if numbers[string(temp[3])] {
			temp = "/"+temp[1:4]+"C "
		} else {
			temp = "/"+temp[1:3]+"C "
		}
	}
	
	
	result = hostname+" "+strconv.Itoa(int(math.Ceil(percent[0])))+"%"+temp+strconv.Itoa(ram_usage)+"/"+strconv.Itoa(ram)+"G";
	return result
}

func sendData(data string) {
	fmt.Println(URI+fmt.Sprintf("?code=%s&str=%s", CODE, data))
	http.Get(URI+fmt.Sprintf("?code=%s&str=%s", CODE, data))
}
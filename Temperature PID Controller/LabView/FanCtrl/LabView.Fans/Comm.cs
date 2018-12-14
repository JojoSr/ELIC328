using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO.Ports;

namespace LabView.Fans
{

    /*
     {
		"command":"update",
		"data":{
			"zones":[
			{
				"minTemp":20.0,
				"maxTemp":30.0,
				"tempSampleRate":1000,
				"ki":0.01,
				"kp":0.01,
				"kd":0.01,
				"pidSampleRate":1000
			},
			{
				"minTemp":20.0,
				"maxTemp":30.0,
				"tempSampleRate":1000,
				"ki":0.01,
				"kp":0.01,
				"kd":0.01,
				"pidSampleRate":1000
			}
		]}
	}



        {\"action\":\"update\",\"data\":{\"zoneUpdate\":[{\"temp\":23.3,"fanRpm":233,"fanPwm":234,"error":-1.4,"pidout":34}]}}
     * */

    public class ComandPacket
    {
        public string command { get; set; }
        public List<zones> data { get; set; }
    }


    public class UpdatePacket
    {
        public string action { get; set; }
        public List<zoneUpdate> data { get; set; }
    }

    public class zoneUpdate
    {
        public double temp { get; set; }
        public int fanRpm { get; set; }

        public int fanPwm { get; set; }

        public int error { get; set; }

        public int pidout { get; set; }
    }


    /// <summary>
    ///  Zones Data
    /// </summary>
    public class zones
    {
        /// <summary>
        /// Minimum Temperature Settings
        /// </summary>
        public double minTemp { get; set; }
        /// <summary> 
        /// Maximum Temperature Settings
        /// </summary>
        public double maxTemp { get; set; }
        /// <summary>
        /// Temperature Sample Rate
        /// </summary>
        public int tempSampleRate { get; set; }
        /// <summary>
        /// Intergal Rate
        /// </summary>
        public double ki { get; set; }
        /// <summary>
        /// Proportional rate
        /// </summary>
        public double kp { get; set; }
        /// <summary>
        /// Dirvitive Rate
        /// </summary>
        public double kd { get; set; }
        /// <summary>
        /// PID sample rate
        /// </summary>
        public int pidSampleRate { get; set; }
    }


    public class ZoneData
    {
        /// <summary>
        /// Minimum Temperature Settings
        /// </summary>
        public double minTemp { get; set; }
        /// <summary> 
        /// Maximum Temperature Settings
        /// </summary>
        public double maxTemp { get; set; }
        /// <summary>
        /// Temperature Sample Rate
        /// </summary>
        public int tempSampleRate { get; set; }
        /// <summary>
        /// Intergal Rate
        /// </summary>
        public double ki { get; set; }
        /// <summary>
        /// Proportional rate
        /// </summary>
        public double kp { get; set; }
        /// <summary>
        /// Dirvitive Rate
        /// </summary>
        public double kd { get; set; }
        /// <summary>
        /// PID sample rate
        /// </summary>
        public int pidSampleRate { get; set; }
        /// <summary>
        /// Current Temperature
        /// </summary>
        public double temp { get; set; }
        /// <summary>
        /// current Fan RPM
        /// </summary>
        public int fanRpm { get; set; }
        /// <summary>
        /// Current Fan PWM Signal
        /// </summary>
        public int fanPwm { get; set; }
        /// <summary>
        /// Current Error 
        /// </summary>
        public double error { get; set; }
        /// <summary>
        /// Current PID Output
        /// </summary>
        public double pidout { get; set; }


        public ZoneData()
        {
            error = 0;
            fanPwm = 0;
            fanRpm = 0;
            kd = 0;
            ki = 0;
            kp = 0;
            maxTemp = 30.0;
            minTemp = 20.0;
            pidout = 0;
            pidSampleRate = 1000;
            temp = 0;
            tempSampleRate = 1000;
        }
    }

    /// <summary>
    /// Communication Layer For LabView
    /// </summary>
    public class Comm
    {
        /// <summary>
        /// List of the Zones
        /// </summary>
        private List<ZoneData> Zones { get; set; }

        public double ZoneOneTemperature => Zones[0].temp;
        public double ZoneTwoTemperature => Zones[1].temp;

        public double ZoneOneError => Zones[0].error;
        public double ZoneTwoError => Zones[1].error;


        public double ZoneOnePidRate => Zones[0].pidout;
        public double ZoneTwoPidRate => Zones[1].pidout;


        public int ZoneOneFanPwm => Zones[0].fanPwm;
        public int ZoneTwoFanPwm => Zones[1].fanPwm;



        /// <summary>
        /// Serial Com Port
        /// </summary>
        public string ComPort { get; set; }

        public string LastData { get; set; }


        private SerialPort _serial = null;


        /// <summary>
        /// Default Constructor
        /// </summary>
        public Comm()
        {
            try
            {
                Zones = new List<ZoneData>();
                ZoneData z1 = new ZoneData();
                ZoneData z2 = new ZoneData();
                Zones.Add(z1);
                Zones.Add(z2);
                ComPort = "COM4";
            }
            catch (Exception es)
            {
                throw new Exception("Constructor Error " + es.StackTrace);
            }
        }


        /// <summary>
        /// Initialize the communication layer
        /// </summary>
        public void InitComm()
        {
            try
            {
                RunBackgroundThread();
            }
            catch (Exception es)
            {
                throw new Exception("Port Open Error " + es.StackTrace);
            }
        }

        public void CloseComm()
        {
            _serial.Close();
        }


        public void RunLoop()
        {
            if (_serial != null && _serial.IsOpen)
            {
                if (_serial.BytesToRead > 0)
                {
                    string data = _serial.ReadTo(";");
                    ReadData(data);
                    LastData = data;
                }
            }
        }


        public void ReadData(string data)
        {

            try
            {
                JObject info = JObject.Parse(data);


                if (info["action"].Value<string>() == "update")
                {
                    // get the data - Parse out zone data
                    JToken z1 = info["data"]["zone"][0];
                    JToken z2 = info["data"]["zone"][1];
                    System.Diagnostics.Debug.WriteLine(z1);

                    // set local data buffer for zone one
                    Zones[0].error = z1["error"].Value<double>();
                    Zones[0].fanPwm = z1["fanPwm"].Value<int>();
                    Zones[0].fanRpm = z1["fanRpm"].Value<int>();
                    Zones[0].pidout = z1["pidout"].Value<double>();
                    Zones[0].temp = z1["temp"].Value<double>();

                    // set the local buffer for zone two
                    Zones[1].error = z2["error"].Value<double>();
                    Zones[1].fanPwm = z2["fanPwm"].Value<int>();
                    Zones[1].fanRpm = z2["fanRpm"].Value<int>();
                    Zones[1].pidout = z2["pidout"].Value<double>();
                    Zones[1].temp = z2["temp"].Value<double>();

                }
            }
            catch { }
        }


        private void RunBackgroundThread()
        {
            _serial = new SerialPort(ComPort)
            {
                BaudRate = 9600,
                DiscardNull = true,
                Parity = Parity.None
            };
            _serial.Open();
        }


        public void SetSettings(int zoneNumber, double minTemp, double maxTemp, int tempSampleRate, double ki, double kp, double kd, int pidSampleRate)
        {
            System.Diagnostics.Debug.WriteLine("Set Values Called");

            if (zoneNumber == 1)
            {
                Zones[0].kd = kd;
                Zones[0].ki = ki;
                Zones[0].kp = kp;
                Zones[0].maxTemp = maxTemp;
                Zones[0].minTemp = minTemp;
                Zones[0].pidSampleRate = pidSampleRate;
                Zones[0].tempSampleRate = tempSampleRate;
            }
            else if (zoneNumber == 2)
            {
                Zones[1].kd = kd;
                Zones[1].ki = ki;
                Zones[1].kp = kp;
                Zones[1].maxTemp = maxTemp;
                Zones[1].minTemp = minTemp;
                Zones[1].pidSampleRate = pidSampleRate;
                Zones[1].tempSampleRate = tempSampleRate;
            }


            // write update to serial port
            string data = "{\"command\":\"update\",\"data\":{\"zones\":[";
            data += "{\"minTemp\":" + Zones[0].minTemp + ",";
            data += "\"maxTemp\":" + Zones[0].maxTemp + ",";
            data += "\"tempSampleRate\":" + Zones[0].tempSampleRate + ",";
            data += "\"ki\":" + Zones[0].ki + ",";
            data += "\"kp\":" + Zones[0].kp + ",";
            data += "\"kd\":" + Zones[0].kd + ",";
            data += "\"pidSampleRate\":" + Zones[0].pidSampleRate;
            data += "},{";
            data += "\"minTemp\":" + Zones[1].minTemp + ",";
            data += "\"maxTemp\":" + Zones[1].maxTemp + ",";
            data += "\"tempSampleRate\":" + Zones[1].tempSampleRate + ",";
            data += "\"ki\":" + Zones[1].ki + ",";
            data += "\"kp\":" + Zones[1].kp + ",";
            data += "\"kd\":" + Zones[1].kd + ",";
            data += "\"pidSampleRate\":" + Zones[1].pidSampleRate;
            data += "}]}	};";

            System.Diagnostics.Debug.WriteLine(data);

            _serial.WriteLine(data);
        }
    }
}

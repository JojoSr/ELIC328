using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Net.Mail;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace mjohansen.labview.email
{

    public class Messages
    {
        public string ToEmailAddress { get; set; }

        public string FromEmailAddress { get; set; }

        public string Message { get; set; }

        public Messages() { }
    }



    public class EmailManager
    {
        private Timer tms;
        public bool HasMessage { get; private set; }

        public Queue<Messages> MessagesQueue { get; set; }

        public string GoogleServerAPI { get; set; }

        public string GoogleSenderId { get; set; }




        public EmailManager()
        {
            tms = new Timer(30000)
            {
                AutoReset = true
            };
            tms.Elapsed += Tms_Elapsed;
            tms.Enabled = true;
            tms.Start();
            GoogleSenderId = "948073604642";
            GoogleServerAPI = "AAAA3L2XJiI:APA91bGNZogyz9pumxU3qpdUWMVBKqPyShBNYmGPnOJGSFJfBZmeRgdxSo6qhMU1hiLaBGVebOc9i7xUbiAOvEkQQPAF_pfIZII7pKIyHAppnM801OIkKBIQt_6DP11PNDRg8lMYopjm";
        }


        public void StartTimer(double interval)
        {
            if (tms != null)
            {
                tms.Enabled = true;
                tms.Interval = interval;
                tms.Start();
            }
        }


        public void StopTimer()
        {
            if (tms != null)
            {
                tms.Enabled = false;
                tms.Stop();
            }
        }


        public void EnQueueMessage(string to, string from, string message)
        {
            Messages msg = new Messages
            {
                FromEmailAddress = from,
                ToEmailAddress = to,
                Message = message
            };

            MessagesQueue.Enqueue(msg);
            HasMessage = true;
        }


        public void SendPushAlarm(string message)
        {
            WebRequest request = WebRequest.Create("https://fcm.googleapis.com/fcm/send");
            request.Method = "POST";
            request.Headers.Add(string.Format("Authorization: key={0}", GoogleServerAPI));
            request.Headers.Add(string.Format("Sender: id={0}", GoogleSenderId));
            request.ContentType = "application/json";
            JObject jfcmData = new JObject();
            JObject jdata = new JObject();

            jfcmData.Add("to", "/topics/alarm");
            jfcmData.Add("priority", "high");
            jdata.Add("title", "Server Temperature Alarm!");
            jdata.Add("body", message);
            jdata.Add("sound", "default");
            jfcmData.Add("notification", jdata);

            Byte[] btArr = Encoding.UTF8.GetBytes(jfcmData.ToString());
            request.ContentLength = btArr.LongLength;
            using (Stream dataStream = request.GetRequestStream())
            {
                dataStream.Write(btArr, 0, btArr.Length);
                using (WebResponse webResponse = request.GetResponse())
                {
                    using (Stream dataStreamResponse = webResponse.GetResponseStream())
                    {
                        using (StreamReader tReader = new StreamReader(dataStreamResponse))
                        {
                            String sResponseFromServer = tReader.ReadToEnd();
                            System.Diagnostics.Debug.WriteLine(sResponseFromServer);
                            System.Diagnostics.Debug.WriteLine(jfcmData);
                        }
                    }
                }
            }
        }



        public void SendPushData(float z1, float z2, float z3, float z4, float z5)
        {

            WebRequest request = WebRequest.Create("https://fcm.googleapis.com/fcm/send");
            request.Method = "POST";
            request.Headers.Add(string.Format("Authorization: key={0}", GoogleServerAPI));
            request.Headers.Add(string.Format("Sender: id={0}", GoogleSenderId));
            request.ContentType = "application/json";
            JObject jfcmData = new JObject();
            JObject jdata = new JObject();

            jfcmData.Add("to", "/topics/data");
            jfcmData.Add("priority", "high");
            jdata.Add("Update", "Update Server Temps.");
            jdata.Add("z1", z1.ToString());
            jdata.Add("z2", z2.ToString());
            jdata.Add("z3", z3.ToString());
            jdata.Add("z4", z4.ToString());
            jdata.Add("z5", z5.ToString());
            jfcmData.Add("data", jdata);

            Byte[] btArr = Encoding.UTF8.GetBytes(jfcmData.ToString());
            request.ContentLength = btArr.LongLength;
            using (Stream dataStream = request.GetRequestStream())
            {
                dataStream.Write(btArr, 0, btArr.Length);
                using (WebResponse webResponse = request.GetResponse())
                {
                    using (Stream dataStreamResponse = webResponse.GetResponseStream())
                    {
                        using (StreamReader tReader = new StreamReader(dataStreamResponse))
                        {
                            String sResponseFromServer = tReader.ReadToEnd();
                            System.Diagnostics.Debug.WriteLine(sResponseFromServer);
                            System.Diagnostics.Debug.WriteLine(jfcmData);
                        }
                    }
                }
            }


        }



        private void Tms_Elapsed(object sender, ElapsedEventArgs e)
        {
            if (HasMessage)
            {
                SmtpClient client = new SmtpClient
                {
                    Credentials = new System.Net.NetworkCredential("m.johansen@mj-2.com", "Shwn@2004"),
                    EnableSsl = false,
                    Host = "mail.mj-2.com",
                    Port = 25
                };

                foreach (Messages m in MessagesQueue)
                {
                    MailMessage msg = new MailMessage
                    {
                        Body = m.Message,
                        From = new MailAddress(m.FromEmailAddress),
                        Subject = "Message From Temperature Monitor"
                    };
                    msg.To.Add(new MailAddress(m.ToEmailAddress));
                    client.Send(msg);
                }
                HasMessage = false;
            }
        }
    }
}

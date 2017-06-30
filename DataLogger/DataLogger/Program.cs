
using System;
using System.IO;
using System.IO.Ports;
using System.Threading;




namespace DataLogger
{
    class DataLogger
    {
        public static byte POINT_INDICATOR_RESET;
        public static byte POINT_INDICATOR_SENSORS;
        public static byte POINT_INDICATOR_GPS;

        public static int POINT_LEN_SENSORS;
        public static int POINT_LEN_GPS;

        public static int POINT_POS_BAR;
        public static int POINT_POS_ACC;
        public static int POINT_POS_HG;
        public static int POINT_POS_GYRO;

        public static int POINT_POS_TIME;
        public static int POINT_POS_LAT;
        public static int POINT_POS_LON;
        public static int POINT_POS_ALT;
        public static int POINT_POS_SATS;
        public static int POINT_POS_FIXQ;

        public static int PAGE_SIZE;

        public static double ACC_LSB;
        public static double HIGH_G_LSB;
        public static double GYRO_LSB;

        public static void Main()
        {
            // Get a list of serial port names.
            string[] ports = SerialPort.GetPortNames();

            Console.WriteLine("The following serial ports were found:");

            // Display each port name to the console.
            foreach (string portx in ports)
            {
                Console.WriteLine(portx);
            }

            Console.WriteLine("\n\nPlease enter number of port to use and press enter:");

            string portNum = Console.ReadLine();

            Console.WriteLine("\nPlease reset trackuino and press enter:");

            Console.ReadLine();

            Console.WriteLine("\nTry to connect...");

            StreamWriter gps_file = new System.IO.StreamWriter("gps0.csv");
            StreamWriter sensors_file = new System.IO.StreamWriter("sensors0.csv");

            // Create the serial port with basic settings
            SerialPort port = new SerialPort("COM" + portNum, 1000000, Parity.None, 8, StopBits.One);

            port.Open();

            port.ReadTimeout = 100;
            string comCheck = "";
            do
            {
                try
                {
                    comCheck = port.ReadLine();
                }
                catch (Exception es) { }
            } while (comCheck != "should i send my data?");

            string line = "";

            port.Write("yes");

            port.ReadTimeout = 1000;

            uint memPos = Convert.ToUInt32(port.ReadLine());
            POINT_INDICATOR_RESET = Convert.ToByte(port.ReadLine());
            POINT_INDICATOR_SENSORS = Convert.ToByte(port.ReadLine());
            POINT_INDICATOR_GPS = Convert.ToByte(port.ReadLine());
            POINT_LEN_SENSORS = Convert.ToInt32(port.ReadLine());
            POINT_LEN_GPS = Convert.ToInt32(port.ReadLine());
            POINT_POS_BAR = Convert.ToInt32(port.ReadLine());
            POINT_POS_ACC = Convert.ToInt32(port.ReadLine());
            POINT_POS_HG = Convert.ToInt32(port.ReadLine());
            POINT_POS_GYRO = Convert.ToInt32(port.ReadLine());
            POINT_POS_TIME = Convert.ToInt32(port.ReadLine());
            POINT_POS_LAT = Convert.ToInt32(port.ReadLine());
            POINT_POS_LON = Convert.ToInt32(port.ReadLine());
            POINT_POS_ALT = Convert.ToInt32(port.ReadLine());
            POINT_POS_SATS = Convert.ToInt32(port.ReadLine());
            POINT_POS_FIXQ = Convert.ToInt32(port.ReadLine());
            ACC_LSB = Convert.ToDouble(port.ReadLine());
            HIGH_G_LSB = Convert.ToDouble(port.ReadLine());
            GYRO_LSB = Convert.ToDouble(port.ReadLine());
            PAGE_SIZE = Convert.ToInt32(port.ReadLine());

            Console.WriteLine("\n" + Convert.ToString(memPos) + " bytes of data in flash (" + Convert.ToString((double)memPos / 32768000.0) + " % of memory used)\n\nReceiving data...\n\n");
            double timestamp_start = DateTime.Now.TimeOfDay.TotalMilliseconds;
            double timestamp_last_console_update = DateTime.Now.TimeOfDay.TotalMilliseconds;
            uint addr = 0;
            uint reset_cnt = 0;
            do
            {
                try
                {
                    line = port.ReadLine();

                    if (line.Contains("page"))
                    {
                        byte[] address = new byte[4];
                        while (port.BytesToRead < 4) { }
                        port.Read(address, 0, 4);
                        addr = ((uint)address[0] << 24) | ((uint)address[1] << 16) | ((uint)address[2] << 8) | address[3];

                        if (DateTime.Now.TimeOfDay.TotalMilliseconds - timestamp_last_console_update > 1000)
                        {
                            timestamp_last_console_update = DateTime.Now.TimeOfDay.TotalMilliseconds;

                            double percents = (double)addr / memPos * 100.0;
                            double remainingTime = (DateTime.Now.TimeOfDay.TotalMilliseconds - timestamp_start) / percents * (100.0 - percents);
                            Console.WriteLine(Convert.ToString(percents) + "% received; remaining time ~ " + Convert.ToString(remainingTime / 60000.0) + " mins");
                        }


                        byte[] page = new byte[PAGE_SIZE];
                        while (port.BytesToRead < PAGE_SIZE) { }
                        port.Read(page, 0, PAGE_SIZE);

                        int i = 0;
                        while (i < PAGE_SIZE - POINT_LEN_GPS && i < PAGE_SIZE - POINT_LEN_SENSORS)
                        {
                            if (page[i] == POINT_INDICATOR_RESET)
                            {
                                gps_file.Close();
                                sensors_file.Close();
                                gps_file = new System.IO.StreamWriter("gps" + Convert.ToString(reset_cnt) + ".csv");
                                gps_file.WriteLine("millis,gps_time,lat,lon,alt,sats,fixq");
                                sensors_file = new System.IO.StreamWriter("sensors" + Convert.ToString(reset_cnt) + ".csv");
                                sensors_file.WriteLine("millis,d1,d2,accx,accy,accz,hgx,hgy,hgz,gyrox,gyroy,gyroz");

                                reset_cnt++;
                                i++;
                            }
                            else if (page[i] == POINT_INDICATOR_GPS)
                            {
                                byte[] point = new byte[POINT_LEN_GPS];
                                Array.Copy(page, i, point, 0, POINT_LEN_GPS);
                                string gps_line = getGpsLine(point);
                                gps_file.WriteLine(gps_line);

                                i += POINT_LEN_GPS;
                            }

                            else if (page[i] == POINT_INDICATOR_SENSORS)
                            {
                                byte[] point = new byte[POINT_LEN_SENSORS];
                                Array.Copy(page, i, point, 0, POINT_LEN_SENSORS);
                                string sensors_line = getSensorsLine(point);
                                sensors_file.WriteLine(sensors_line);

                                i += POINT_LEN_SENSORS;
                            }

                            else { i++; }

                        }
                    }
                }
                catch (Exception es)
                {
                    if (addr > memPos - 500) break;
                }
            } while (true);

            port.Close();
            gps_file.Close();
            sensors_file.Close();

            Console.WriteLine("\nDone\n\nExit by pressing enter...");
            Console.ReadLine();
        }

        public static string getSensorsLine(byte[] data)
        {
            uint millis = ((uint)data[1] << 24) | ((uint)data[2] << 16) | ((uint)data[3] << 8) | data[4];
            uint D1 = (uint)data[POINT_POS_BAR] << 16 | (uint)data[POINT_POS_BAR + 1] << 8 | data[POINT_POS_BAR + 2];
            uint D2 = (uint)data[POINT_POS_BAR + 3] << 16 | (uint)data[POINT_POS_BAR + 4] << 8 | data[POINT_POS_BAR + 5];

            byte help = data[POINT_POS_ACC];
            data[POINT_POS_ACC] = data[POINT_POS_ACC + 1];
            data[POINT_POS_ACC + 1] = help;
            short accx = BitConverter.ToInt16(data, POINT_POS_ACC);

            help = data[POINT_POS_ACC + 2];
            data[POINT_POS_ACC + 2] = data[POINT_POS_ACC + 3];
            data[POINT_POS_ACC + 3] = help;
            short accy = BitConverter.ToInt16(data, POINT_POS_ACC + 2);

            help = data[POINT_POS_ACC + 4];
            data[POINT_POS_ACC + 4] = data[POINT_POS_ACC + 5];
            data[POINT_POS_ACC + 5] = help;
            short accz = BitConverter.ToInt16(data, POINT_POS_ACC + 4);

            help = data[POINT_POS_HG];
            data[POINT_POS_HG] = data[POINT_POS_HG + 1];
            data[POINT_POS_HG + 1] = help;
            short hgx = BitConverter.ToInt16(data, POINT_POS_HG);

            help = data[POINT_POS_HG + 2];
            data[POINT_POS_HG + 2] = data[POINT_POS_HG + 3];
            data[POINT_POS_HG + 3] = help;
            short hgy = BitConverter.ToInt16(data, POINT_POS_HG + 2);

            help = data[POINT_POS_HG + 4];
            data[POINT_POS_HG + 4] = data[POINT_POS_HG + 5];
            data[POINT_POS_HG + 5] = help;
            short hgz = BitConverter.ToInt16(data, POINT_POS_HG + 4);

            help = data[POINT_POS_GYRO];
            data[POINT_POS_GYRO] = data[POINT_POS_GYRO + 1];
            data[POINT_POS_GYRO + 1] = help;
            short gyrox = BitConverter.ToInt16(data, POINT_POS_GYRO);

            help = data[POINT_POS_GYRO + 2];
            data[POINT_POS_GYRO + 2] = data[POINT_POS_GYRO + 3];
            data[POINT_POS_GYRO + 3] = help;
            short gyroy = BitConverter.ToInt16(data, POINT_POS_GYRO + 2);

            help = data[POINT_POS_GYRO + 4];
            data[POINT_POS_GYRO + 4] = data[POINT_POS_GYRO + 5];
            data[POINT_POS_GYRO + 5] = help;
            short gyroz = BitConverter.ToInt16(data, POINT_POS_GYRO + 4);

            string sensorLine = Convert.ToString(millis) + "," + Convert.ToString(D1) + "," + Convert.ToString(D2) 
                + "," + Convert.ToString((float)accx * ACC_LSB) + "," + Convert.ToString((float)accy * ACC_LSB) + "," + Convert.ToString((float)accz * ACC_LSB) 
                + "," + Convert.ToString((float)hgx * HIGH_G_LSB) + "," + Convert.ToString((float)hgy * HIGH_G_LSB) + "," + Convert.ToString((float)hgz * HIGH_G_LSB) + "," 
                + Convert.ToString((float)gyrox * GYRO_LSB) + "," + Convert.ToString((float)gyroy * GYRO_LSB) + "," + Convert.ToString((float)gyroz * GYRO_LSB);

            return sensorLine;
        }


        public static string getGpsLine(byte[] data)
        {
            uint millis = ((uint)data[1] << 24) | ((uint)data[2] << 16) | ((uint)data[3] << 8) | data[4];
            string time = System.Text.Encoding.ASCII.GetString(data, POINT_POS_TIME, 6);
            string lat = System.Text.Encoding.ASCII.GetString(data, POINT_POS_LAT, 11);
            string lon = System.Text.Encoding.ASCII.GetString(data, POINT_POS_LON, 12);
            float alt = System.BitConverter.ToSingle(data, POINT_POS_ALT);
            byte sats = data[POINT_POS_SATS];
            byte fixq = data[POINT_POS_FIXQ];

            string gpsLine = Convert.ToString(millis) + "," + time + "," + lat + "," + lon + "," + alt + "," + sats + "," + fixq;

            return gpsLine;
        }
    }
}
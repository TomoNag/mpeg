using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Web;

using System.Text.RegularExpressions;


using System.Xml;
using System.Xml.Serialization;
using System.Net;
using System.IO;

using mshtml;
using System.Collections;

using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;

using FFmpegWrapper;
using FFmpeg;


using OpenCV;
using System.Runtime.InteropServices;
using NReco.VideoConverter;
using TheArtOfDev.HtmlRenderer.WinForms;
using System.Diagnostics;
//https://htmlrenderer.codeplex.com/documentation

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        public WebBrowser webBrowser1 = new WebBrowser();
        public Form1()
        {
            InitializeComponent();

            webBrowser1.Width = pictureBox1.Width;
            webBrowser1.Height = pictureBox1.Height;
            webBrowser1.ScrollBarsEnabled = false;

            webBrowser1.DocumentCompleted += WebBrowser1_DocumentCompleted;
            webBrowser1.Navigated += WebBrowser1_Navigated;
            FFmpegWrapper.Commands.form = this;

            this.Load += Form1_Load;

            this.AllowDrop = true;
            this.DragEnter += Form1_DragEnter;
            this.DragDrop += Form1_DragDrop;
        }

    
        private void Form1_DragDrop(object sender, DragEventArgs e)
        {

            string[] fileName =
                (string[])e.Data.GetData(DataFormats.FileDrop, false);

            if (fileName.Length > 0)
            {
                Create(fileName[0]);
            }
            
        }
        string tempout = "temp.mp4";
        string output = "Result.mp4";

        private static string Execute(string exePath, string parameters)
        {
            string result = String.Empty;

            using (Process p = new Process())
            {
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.CreateNoWindow = true;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.FileName = exePath;
                p.StartInfo.Arguments = parameters;
                p.Start();
                p.WaitForExit();

                result = p.StandardOutput.ReadToEnd();
            }

            return result;
        }

        public void Create(string input)
        {
         
            if(File.Exists(output))
            {
                File.Delete(output);
            }

            transparent.Save("logo.png", System.Drawing.Imaging.ImageFormat.Png);

            var setting = new NReco.VideoConverter.ConvertSettings();
            setting.SetVideoFrameSize(1280, 720);
            setting.CustomInputArgs = "-ss 00:00:00 -t 00:00:05";
            // setting.CustomOutputArgs = "-vf vflip";
            setting.CustomOutputArgs = "-vf \"movie=logo.png [watermark]; [in][watermark] overlay=10:10 [out]\"";
            //setting.CustomOutputArgs = "-vf \"setsar=sar=1/1\"";

            var ffMpeg = new NReco.VideoConverter.FFMpegConverter();
        
            ffMpeg.ConvertMedia(input, null, tempout, NReco.VideoConverter.Format.mp4, setting);

            string cmdParams = "-i opening.mp4 -i temp.mp4 -filter_complex \"[0:0] [1:0] concat=n=2:v=1:a=0:unsafe=1\" " + output;

            try {
                Execute("ffmpeg.exe", cmdParams);

              
                MessageBox.Show("FINISHED");
            }
            catch
            {
                MessageBox.Show("結合失敗。");
            }


            //NReco.VideoConverter.ConcatSettings set = new NReco.VideoConverter.ConcatSettings();
            //set.ConcatAudioStream = false;

            ////set.SetVideoFrameSize(640, 480);
            //string[] inputfiles = { "opening.mp4", tempout };
            //ffMpeg.ConcatMedia(inputfiles, output, NReco.VideoConverter.Format.mp4, set);
            //this.Text = "FINISH";
        }
        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            //コントロール内にドラッグされたとき実行される
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                //ドラッグされたデータ形式を調べ、ファイルのときはコピーとする
                e.Effect = DragDropEffects.Copy;
            else
                //ファイル以外は受け付けない
                e.Effect = DragDropEffects.None;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            BlackBg = true;
            DrawWeb();
           
        }

        public bool BlackBg = true;
        private void WebBrowser1_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            IHTMLDocument2 doc = (webBrowser1.Document.DomDocument) as IHTMLDocument2;
            IHTMLStyleSheet ss = doc.createStyleSheet("", 0);

            if (BlackBg == true)
            {
                ss.cssText = "body {background-color: #000000;}"; // GetCssText();
            }
            else
            {
                ss.cssText = "body {background-color: #FFFFFF;}";
            }
        }

        public Bitmap black;
        public Bitmap white;
        public Bitmap transparent;

        public double calc(double red1, double red2, double alpha)
        {
            double red = ((red1 + red2) / 2 - 127.5) / alpha + 127.5;

            return red1;
        }


        public Bitmap Marge(Bitmap white, Bitmap black)
        {
            BitmapData sourceData = white.LockBits
                           (new Rectangle(0, 0, white.Width, white.Height),
                           ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            byte[] resultBuffer = new byte[sourceData.Stride * sourceData.Height];
            Marshal.Copy(sourceData.Scan0, resultBuffer, 0, resultBuffer.Length);
            white.UnlockBits(sourceData);

            BitmapData sourceData2 = black.LockBits
                          (new Rectangle(0, 0, black.Width, black.Height),
                          ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            byte[] resultBuffer2 = new byte[sourceData2.Stride * sourceData2.Height];
            Marshal.Copy(sourceData2.Scan0, resultBuffer2, 0, resultBuffer2.Length);
            black.UnlockBits(sourceData2);

            int sourceBlue = 0, resultBlue = 0,
                    sourceGreen = 0, resultGreen = 0,
                    sourceRed = 0, resultRed = 0,
            resultAlpha = 0;
          

            for (int k = 0; k < resultBuffer.Length; k += 4)
            {
                int blue1 = resultBuffer[k];
                int blue2 = resultBuffer2[k];

                int green1 = resultBuffer[k + 1];
                int green2 = resultBuffer2[k + 1];

                int red1 = resultBuffer[k + 2];
                int red2 = resultBuffer2[k + 2];

                resultAlpha = (255 * 3 - (blue1 - blue2) - (red1 - red2) - (green1 - green2)) / 3;

                if(resultAlpha == 0)
                {
                    resultBlue = 0;
                    resultRed = 0;
                    resultGreen = 0;
                }
                else if (resultAlpha == 255)
                {
                    resultBlue = blue1;
                    resultRed = red1;
                    resultGreen = green1;
                }
                else 
                {
                    double alpha = (double)resultAlpha / 255;


                    double blue = calc(blue1, blue2, alpha); //(blue1 + blue2) / (2 * alpha);
                    double red = calc(red1, red2, alpha);
                    double green = calc(green1, green2, alpha);

                    resultBlue = (int)blue;
                    resultGreen = (int)green;
                    resultRed = (int)red;
                }
               

                

                resultBuffer[k] = (byte)resultBlue;
                resultBuffer[k + 1] = (byte)resultGreen;
                resultBuffer[k + 2] = (byte)resultRed;
                resultBuffer[k + 3] = (byte)resultAlpha;
            }


            Bitmap resultBitmap = new Bitmap(white.Width, white.Height,
                                                PixelFormat.Format32bppArgb);

            BitmapData resultData = resultBitmap.LockBits
                                    (new Rectangle(0, 0, resultBitmap.Width, resultBitmap.Height),
                                    ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);


            Marshal.Copy(resultBuffer, 0, resultData.Scan0, resultBuffer.Length);
            resultBitmap.UnlockBits(resultData);


            return resultBitmap;
        }

        private void WebBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            Application.DoEvents();

            Control c = this.webBrowser1;
            System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(c.Width, c.Height);
            c.DrawToBitmap(bmp, c.ClientRectangle);

            

            if (BlackBg == true)
            {
                black = bmp;
                BlackBg = false;
                DrawWeb();
            }
            else
            {
                white = bmp;
                transparent = Marge(white, black);

                pictureBox1.Image = transparent;
                //pictureBox2.Image = black;
                //pictureBox3.Image = white;
                BlackBg = true;
            }
        }

        double request_time = 0.0;
        public Bitmap frame;
        public void SetImage(Bitmap bmp)
        {
            if(this.pictureBox1.Image != null)
            this.pictureBox1.Image.Dispose();


            //Image image = HtmlRender.RenderToImage(html, new Size(400, 200), Color.Linen);

            this.pictureBox1.Image = DeepCopy(bmp);
            Application.DoEvents();
        }

        public Bitmap DeepCopy(Bitmap B)
        {
            return new Bitmap(B);

            return B.Clone(new Rectangle(0, 0, B.Width, B.Height), B.PixelFormat);
        }
        private void button1_Click(object sender, EventArgs e)
        {
            string input = "D:\\MUSIC\\ビデオ８\\britney spears - overprotected - lyrics.mp4";

            string output = "C:\\test3.avi";


            Commands.GetFrame("test.mp4", request_time);
            request_time++;

            //System.IO.FileStream hStream = new System.IO.FileStream(@"frame.buffer.jpg", System.IO.FileMode.Open);
            //this.pictureBox1.Image = Image.FromStream(hStream);
            //hStream.Close();
            return;
            //this.Text = Class1.testX("123あいうえお").ToString(); // testX().ToString();
           
        
            if (System.IO.File.Exists(input))
            {
                OpenCV.VideoEditor.test(input, output);
            }

            //Class1.video(input, output);

           
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string input = @"C:\Users\a\Videos\Debut\Untitled 7.avi";

            string output = "C:\\test3.avi";

            var setting = new NReco.VideoConverter.ConvertSettings();
            setting.SetVideoFrameSize(1280, 720);
            setting.CustomInputArgs = "-ss 00:00:00 -t 00:00:10";
           // setting.CustomOutputArgs = "-vf vflip";
           setting.CustomOutputArgs = "-vf \"movie=logo.png [watermark]; [in][watermark] overlay=main_w-overlay_w-10:main_h-overlay_h-10 [out]\"";


            //            Top left corner
            //ffmpeg -i inputvideo.avi -vf "movie=watermarklogo.png [watermark]; [in][watermark] overlay=10:10 [out]" outputvideo.flv

            //Top right corner
            //ffmpeg -i inputvideo.avi -vf "movie=watermarklogo.png [watermark]; [in][watermark] overlay=main_w-overlay_w-10:10 [out]" outputvideo.flv

            //Bottom left corner
            //ffmpeg -i inputvideo.avi -vf "movie=watermarklogo.png [watermark]; [in][watermark] overlay=10:main_h-overlay_h-10 [out]" outputvideo.flv

            //Bottom right corner
            //ffmpeg -i inputvideo.avi -vf "movie=watermarklogo.png [watermark]; [in][watermark] overlay=main_w-overlay_w-10:main_h-overlay_h-10 [out]" outputvideo.flv


           

            var ffMpeg = new NReco.VideoConverter.FFMpegConverter();
            ffMpeg.ConvertProgress += FfMpeg_ConvertProgress;

            ffMpeg.GetVideoThumbnail(input, "temp.jpg", 10.0f);
            System.IO.FileStream hStream = new System.IO.FileStream("temp.jpg", System.IO.FileMode.Open);
            this.pictureBox1.Image = Image.FromStream(hStream);
            // FileStream を閉じる (正しくは オブジェクトの破棄を保証する を参照)
            hStream.Close();
            return;

            //ffMpeg.ConvertMedia(input, null, output, null, setting);

            ffMpeg.ConvertMedia(input, null, output, NReco.VideoConverter.Format.mp4, setting);
            //


            NReco.VideoConverter.ConcatSettings set = new NReco.VideoConverter.ConcatSettings();
            set.SetVideoFrameSize(640, 480);
            //ffMpeg.ConcatMedia(_fileNames, videoRootPath + tobename + ".mp4", NReco.VideoConverter.Format.mp4, set);
            
            this.Text = "FINISH";
        }

        private void FfMpeg_ConvertProgress(object sender, ConvertProgressEventArgs e)
        {
            //this.Text = e.Processed.ToString();
            //Application.DoEvents();
        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            DrawWeb();
        }

        public void DrawWeb()
        {
            webBrowser1.DocumentText = richTextBox1.Text;

        }

        private void button1_Click_1(object sender, EventArgs e)
        {

          
          

            

                var ffMpeg = new NReco.VideoConverter.FFMpegConverter();

                    NReco.VideoConverter.ConcatSettings set = new NReco.VideoConverter.ConcatSettings();
            set.SetVideoFrameSize(640, 480);
            set.ConcatAudioStream = false;
            set.ConcatVideoStream = true;
            //set.CustomOutputArgs = "-filter_complex '[0:v] setsar=sar=1 [in1]; [1:v] setsar=sar=1 [in2]; [in1][in2] concat [v]; [0:a][1:a] concat=v=0:a=1 [a]' -map '[v]' -map '[a]'";
            //set.CustomOutputArgs = "-filter_complex \"[0:0] setsar=1/1[sarfix];[sarfix] [0:1] [1:0] [1:1] concat=n=2:v=1:a=1 [v] [a] \" -map \"[v]\" -map \"[a]\"";
            string[] inputfiles = { "opening.mp4", tempout};
            ffMpeg.ConcatMedia(inputfiles, output, NReco.VideoConverter.Format.mp4, set);
            MessageBox.Show("FINISHED");
        }
    }


   
}

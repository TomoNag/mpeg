using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using FFmpegWrapper;
using FFmpeg;


using OpenCV;
using System.Runtime.InteropServices;
using NReco.VideoConverter;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            FFmpegWrapper.Commands.form = this;
        }

        double request_time = 0.0;
        public Bitmap frame;
        public void SetImage(Bitmap bmp)
        {
            if(this.pictureBox1.Image != null)
            this.pictureBox1.Image.Dispose();
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
            //setting.SetVideoFrameSize(640, 480);
            setting.CustomInputArgs = "-ss 00:00:00 -t 00:00:03";
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
    }


   
}

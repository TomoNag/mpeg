using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Shapes;
using System.Windows.Controls;
using System.Windows.Input;
using System.ComponentModel;

namespace Util
{
    public class MultiSliderPoint : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
    {
        private const double defaultWidth = 10;
        private const double defaultHeight = 10;

        internal Polygon Polygon { get; private set; }
        internal double Left
        {
            get { return (double)Polygon.GetValue(Canvas.LeftProperty); }
            set
            {
                var changing = value == this.Left;
                if (changing)
                    OnPropertyChanging(new System.ComponentModel.PropertyChangingEventArgs("Left"));
                Polygon.SetValue(Canvas.LeftProperty, value);
                if (changing)
                    OnPropertyChanged(new System.ComponentModel.PropertyChangedEventArgs("Left"));
            }
        }
        internal double Center
        {
            get { return this.Left + Width / 2; }
            set
            {
                var changing = value == this.Center;
                if (changing)
                    OnPropertyChanging(new System.ComponentModel.PropertyChangingEventArgs("Center"));
                this.Left = value - Width / 2;
                if (changing)
                    OnPropertyChanged(new System.ComponentModel.PropertyChangedEventArgs("Center"));
            }
        }
        internal double Width
        {
            get
            {
                return (double)Polygon.GetValue(Polygon.WidthProperty);
            }
            set
            {
                var changing = value == Polygon.Width;
                if (changing)
                    OnPropertyChanging(new System.ComponentModel.PropertyChangingEventArgs("Width"));
                Polygon.SetValue(Polygon.WidthProperty, value);
                if (changing)
                    OnPropertyChanged(new System.ComponentModel.PropertyChangedEventArgs("Width"));
            }
        }
        internal double Height
        {
            get
            {
                return (double)Polygon.GetValue(Polygon.HeightProperty);
            }
            set
            {
                var changing = value == this.Width;
                if (changing)
                    OnPropertyChanging(new System.ComponentModel.PropertyChangingEventArgs("Height"));
                Polygon.SetValue(Polygon.HeightProperty, value);
                if (changing)
                    OnPropertyChanged(new System.ComponentModel.PropertyChangedEventArgs("Height"));
            }
        }
        internal Panel Parent { get; set; }
        internal bool IsMouseCaptured { get { return (bool)Polygon.GetValue(Polygon.IsMouseCapturedProperty); } }
        internal bool IsMouseOver { get { return (bool)Polygon.GetValue(Polygon.IsMouseOverProperty); } }
        internal Style Style
        {
            get
            {
                return (Style)Polygon.GetValue(Polygon.StyleProperty);
            }
            set
            {
                var changing = value == this.Style;
                if (changing)
                    OnPropertyChanging(new System.ComponentModel.PropertyChangingEventArgs("Style"));
                Polygon.SetValue(Polygon.StyleProperty, value);
                if (changing)
                    OnPropertyChanged(new System.ComponentModel.PropertyChangedEventArgs("Style"));
            }
        }

        public MultiSliderPoint(double center, Panel parent)
        {
            this.Parent = parent;
            this.Polygon = new Polygon { Width = defaultWidth, Height = defaultHeight };
            if (parent != null)
                this.Center = center;
            Polygon.Points.Add(new Point(0, 10));
            Polygon.Points.Add(new Point(5, 0));
            Polygon.Points.Add(new Point(10, 10));
            Polygon.Points.Add(new Point(0, 10));
            Polygon.MouseMove += Polygon_MouseMove;
            Polygon.MouseUp += Polygon_MouseUp;
            Polygon.MouseDown += Polygon_Down;
            Polygon.Focusable = true;
        }

        ~MultiSliderPoint()
        {
            try { this.Parent.Children.Remove(Polygon); }
            catch { }
        }

        private void Polygon_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton != MouseButtonState.Pressed) return;
            var poly = sender as Polygon;
            var point = Mouse.GetPosition(this.Parent);
            var center = point.X;
            if (center >= 0 && center <= this.Parent.Width)
                this.Center = center;
        }

        private void Polygon_MouseUp(object sender, MouseEventArgs e)
        {
            var poly = sender as Polygon;
            poly.ReleaseMouseCapture();
        }

        private void Polygon_Down(object sender, MouseEventArgs e)
        {
            var poly = sender as Polygon;
            poly.CaptureMouse();
            poly.Focus();
        }

        #region INotifyPropertyChanged Members

        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, e);
        }

        #endregion

        #region INotifyPropertyChanging Members

        public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
        protected virtual void OnPropertyChanging(System.ComponentModel.PropertyChangingEventArgs e)
        {
            if (PropertyChanging != null)
                PropertyChanging(this, e);
        }
        #endregion
    }
}

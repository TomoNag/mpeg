using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;

namespace Util
{
    /// <summary>
    /// Interaction logic for MultiSlider.xaml
    /// </summary>
    public partial class MultiSlider : UserControl
    {
        public event RoutedEventHandler ValuesChanged;
        public event RoutedEventHandler ValueAdded;
        public event EventHandler<ValueRemovedEventArgs> ValueRemoved;

        public static readonly DependencyProperty SliderStyleProperty = DependencyProperty.Register("SliderStyle", typeof(Style), typeof(MultiSlider));
        public static readonly DependencyProperty ValuesProperty = DependencyProperty.Register("Values", typeof(IEnumerable<double>), typeof(MultiSlider), new PropertyMetadata());
        public Style SliderStyle { get { return (Style)GetValue(SliderStyleProperty); } set { SetValue(SliderStyleProperty, value); } }

        private System.Collections.ObjectModel.ObservableCollection<MultiSliderPoint> points;
        public IEnumerable<double> Values
        {
            get
            {
                return points.Select(q => q.Center / canvas.Width);
            }
            set
            {
                points.ToList().ForEach(q => OnRemoveItem(q));
                points.Clear();
                if (value != null)
                    value.Select(q => new MultiSliderPoint(q * canvas.Width, canvas)).ToList().ForEach(q => points.Add(q));
            }
        }
        public int SelectedIndex { get; set; }

        public MultiSlider()
        {
            InitializeComponent();
            points = new System.Collections.ObjectModel.ObservableCollection<MultiSliderPoint>();
            points.CollectionChanged += Values_CollectionChanged;
            InitializeSliderStyle();
        }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);
        }

        private void InitializeSliderStyle()
        {
            SliderStyle = new Style();
            var brush = new LinearGradientBrush(Colors.LightGray, Color.FromRgb((byte)(Colors.LightGray.R * 0.5), (byte)(Colors.LightGray.G * 0.5), (byte)(Colors.LightGray.B * 0.5)), 0);
            SliderStyle.Setters.Add(new Setter(Polygon.FillProperty, brush));
            brush = new LinearGradientBrush(Colors.Gray, Color.FromRgb((byte)(Colors.Gray.R * 0.5), (byte)(Colors.Gray.G * 0.5), (byte)(Colors.Gray.B * 0.5)), 0);
            var trigger = new Trigger { Setters = { new Setter(Polygon.FillProperty, brush) }, Value = true, Property = Polygon.IsFocusedProperty };
            SliderStyle.Triggers.Add(trigger);
        }

        private void OnAddItem(MultiSliderPoint item)
        {
            item.Parent = canvas;
            item.Polygon.Style = SliderStyle;
            item.Polygon.KeyDown += Polygon_KeyDown;
            item.PropertyChanged += Point_PropertyChanged;
            try
            {
                canvas.Children.Add(item.Polygon);
            }
            catch { }
        }

        private void OnRemoveItem(MultiSliderPoint item)
        {
            item.Polygon.KeyDown -= Polygon_KeyDown;
            item.PropertyChanged -= Point_PropertyChanged;
            try
            {
                canvas.Children.Remove(item.Polygon);
            }
            catch { }
        }

        private void Polygon_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key != Key.Delete) return;
            var poly = sender as Polygon;
            if (poly == null || !poly.IsFocused) return;
            var point = points.FirstOrDefault(q => q.Polygon == poly);
            if (point == null) return;
            var idx = points.IndexOf(point);
            points.Remove(point);
            OnValueRemoved(new ValueRemovedEventArgs(idx));
        }

        private void Point_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName.Equals("left", StringComparison.OrdinalIgnoreCase))
                OnValuesChanged(new RoutedEventArgs());
        }

        private void canvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (points.Any(q => q.IsMouseOver)) return;
            var mousePoint = Mouse.GetPosition(sender as IInputElement);
            var point = new MultiSliderPoint(mousePoint.X, canvas);
            point.Polygon.MouseDown += (s, a) => SelectedIndex = points.IndexOf(point);
            if (point.Center >= 0 && point.Center <= canvas.Width)
            {
                points.Add(point);
                point.Polygon.Focus();
                SelectedIndex = points.IndexOf(point);
                OnValueAdded(e);
            }
        }

        private void Values_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            switch (e.Action)
            {
                case System.Collections.Specialized.NotifyCollectionChangedAction.Remove:
                    e.OldItems.Cast<MultiSliderPoint>().ToList().ForEach(q => OnRemoveItem(q));
                    break;
                case System.Collections.Specialized.NotifyCollectionChangedAction.Add:
                    e.NewItems.Cast<MultiSliderPoint>().ToList().ForEach(q => OnAddItem(q));
                    break;
            }
            OnValuesChanged(new RoutedEventArgs());
        }

        protected virtual void OnValuesChanged(RoutedEventArgs e)
        {
            if (ValuesChanged != null && this.IsLoaded)
                ValuesChanged(this, e);
        }

        protected virtual void OnValueAdded(RoutedEventArgs e)
        {
            if (ValueAdded != null && this.IsLoaded)
                ValueAdded(this, e);
        }

        protected virtual void OnValueRemoved(ValueRemovedEventArgs e)
        {
            if (ValueRemoved != null && this.IsLoaded)
                ValueRemoved(this, e);
        }

        #region RemoveEventArgs
        public class ValueRemovedEventArgs : EventArgs
        {
            public int Index { get; private set; }
            public ValueRemovedEventArgs(int index)
            {
                this.Index = index;
            }
        }
        #endregion RemoveEventArgs
    }
}

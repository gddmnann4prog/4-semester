using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CLT_Box_Muller
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            this.chart1.Series.RemoveAt(0);
            this.chart1.Series.Add("ЦПТ");
            this.chart2.Series.RemoveAt(0);
            this.chart2.Series.Add("Бокс-Маллер");
        }

        uint randValue = 0;
        const double expect = 0.5;
        const double disp = 1 / 12.0;

        private double PRNG()
        {
            double value = 0;

            uint M = 2147483647; //2^31-1
            uint k = 1220703125;
            uint b = 7;

            randValue = (k * randValue + b) % M;

            value = randValue / (double)M;

            return value;
        }

        private double Power(double value, int power)
        {
            double result = 0;

            for (int i = 1; i <= power; i++)
            {
                if (i == 1) result = 1;

                result *= result;
            }

            return result;
        }

        private double distribution(double value)
        {
            double result = 0;

            result = value - Power(value, 3) / 6.0 + Power(value, 5) / 40.0;

            return result;
        }

        private double pirsonCalc(int N, int interv, double[] gistValues, double[] gistAmount)
        {
            double result = 0;
            double probability = 0;
            double expected = 0;

            for (int i = 0; i < interv - 1; i++)
            {
                expected = distribution(gistValues[i + 1]) - distribution(gistValues[i]);
                probability = (gistAmount[i]/N - expected) * (gistAmount[i]/N - expected) / expected;

                result += probability;
            }


            return result;
        }

        double PartialSum(double[] selection, int k)
        {
            double result = 0;

            for (int i = 0; i < k; i++)
            {
                result += selection[i];
            }

            return result;
        }

        double GetMaxValue(double[] selection, int N)
        {
            double result = Double.MinValue;

            for (int i = 0; i < N; i++)
            {
                if (i == 0)
                {
                    result = selection[i];
                    continue;
                }
                
                if (result < selection[i])
                {
                    result = selection[i];
                }
            }

            return result;
        }

        double GetMinValue(double[] selection, int N)
        {
            double result = Double.MaxValue;

            for (int i = 0; i < N; i++)
            {
                if (i == 0)
                {
                    result = selection[i];
                    continue;
                }

                if (result > selection[i])
                {
                    result = selection[i];
                }
            }

            return result;
        }

        double[] CLTExpression(double[] selection, int N)
        {
            int amount = 5;
            double PartSum;
            double[] result = new double[N];
            Random rnd = new Random();
            randValue = (uint)rnd.Next();

            for (int i = 0; i < N; i++)
            {
                PartSum = 0;
                for(int j = 0; j < amount; j++)
                {
                    randValue = (uint)(rnd.Next() % N);
                    PartSum += selection[randValue];
                }

                result[i] = PartSum - amount * expect;
                result[i] /= Math.Sqrt(amount * disp);
            }

            return result;
        }

        double[] BoxMullerExpression(double[] selection, int N)
        {
            double[] result = new double[N];

            for (int i = 0; i < N/2; i++)
            {
                result[2 * i] = Math.Sqrt(-2 * Math.Log(selection[2 * i])) * Math.Cos(2 * Math.PI * selection[2 * i + 1]);
                result[2 * i + 1] = Math.Sqrt(-2 * Math.Log(selection[2 * i])) * Math.Sin(2 * Math.PI * selection[2 * i + 1]);
            }

            return result;
        }
        
        private void RenderGist(
            System.Windows.Forms.DataVisualization.Charting.Chart chart,
            TextBox pirsonBox,
            string name,
            double[] selection,
            int N
            )
        {
            int gistIntervals = (int)this.numericUpDown2.Value;
            int currentIndex;
            double pirsonValue;
            double[] gistValues = new double[gistIntervals];
            double[] gistAmount = new double[gistIntervals];

            double minValue = GetMinValue(selection, N);
            double maxValue = GetMaxValue(selection, N);

            for (int i = 0; i < gistIntervals; i++)
            {
                gistValues[i] = minValue + (i * (maxValue - minValue) / (double)gistIntervals);
                gistAmount[i] = 0;
            }

            for (int i = 0; i < N; i++)
            {
                if (selection[i] == maxValue)
                {
                    gistAmount[gistIntervals - 1] += 1;
                }
                else
                {
                    currentIndex = (int)(((selection[i] - minValue) / (double)(maxValue - minValue)) * gistIntervals);
                    gistAmount[currentIndex] += 1;
                }
            }

            chart.Series[name].Points.DataBindXY(gistValues, gistAmount);

            pirsonValue = pirsonCalc(N, gistIntervals, gistValues, gistAmount);
            pirsonBox.Text = pirsonValue.ToString();
        }

        private double[] GenerateSelection(int N)
        {
            double[] selection = new double[N];
            Random rnd = new Random();
            randValue = (uint)rnd.Next();

            for (int i = 0; i < N; i++)
            {
                selection[i] = PRNG();
            }

            return selection;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int N = (int)this.numericUpDown1.Value;
            double[] selection;
            double[] selectionCLT;
            double[] selectionBM;

            selection = GenerateSelection(N);

            selectionCLT = CLTExpression(selection, N);
            selectionBM = BoxMullerExpression(selection, N);

            RenderGist(this.chart1, this.textBox1, "ЦПТ", selectionCLT, N);
            RenderGist(this.chart2, this.textBox2, "Бокс-Маллер", selectionBM, N);
        }
    }
}

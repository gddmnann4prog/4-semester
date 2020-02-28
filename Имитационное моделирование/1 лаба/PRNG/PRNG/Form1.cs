using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PRNG
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        uint randValue = 0;

        private double PRNG()
        {
            double result = 0;
            double value = 0;

            uint M = 2147483647; //2^31-1
            uint k = 1220703125;
            uint b = 7;

            randValue = (k * randValue + b) % M;

            value = randValue / (double)M;

            result = 1.0 - Math.Sqrt(1.0 - value);

            return result;
        }

        private double pirsonCalc(int N, int interv, double[] values, double[] amount)
        {
            double result = 0;

            double[] prob = new double[interv];

            prob[0] = 2 * values[0] - values[0] * values[0];
            for (int i = 0; i < interv-1; i++)
            {
                prob[i] = (2 * values[i+1] - values[i+1] * values[i+1]) - (2 * values[i] - values[i] * values[i]);
                result += (amount[i] - prob[i]*interv) * (amount[i] - prob[i]*interv) * N / (double)(prob[i] * interv * interv);
            }


            return result;
        }


        private void button1_Click(object sender, EventArgs e)
        {
            Random rnd = new Random();
            randValue = (uint)rnd.Next();

            int interv = (int)this.numericUpDown2.Value;
            int N = (int)this.numericUpDown1.Value;

            double[] values = new double[interv];
            double[] amount = new double[interv];
            double currValue = 0;
            double pirson = 0;

            for (int i = 0; i < interv; i++)
            {
                values[i] = i / (double)interv;
                amount[i] = 0;
            }

            for (int i = 0; i < N; i++)
            {
                currValue = PRNG();
                currValue *= interv;
                amount[(int)currValue] += 1;
            }

            for (int i = 0; i < interv; i++)
                amount[i] /= (double)N/(double)interv;

            this.chart1.Series["Series1"].Points.DataBindXY(values, amount);

            pirson = pirsonCalc(N, interv, values, amount);

            this.textBox1.Text = pirson.ToString();
        }

    }
}

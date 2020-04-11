using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Globalization;

namespace Interpolation
{
    public partial class Form1 : Form
    {
        private int[] X = { 0, 0, 0, 0, 0 };
        private int[] FX = { 0, 0, 0, 0, 0 };
        public Form1()
        {
            InitializeComponent();
            chart1.Series.Add("Теоретический");
            chart2.Series.Add("Теоретический");
            chart3.Series.Add("Теоретический");
            chart4.Series.Add("Теоретический");
            chart5.Series.Add("Теоретический");
        }

        void GetFunctionValues()
        {
            X[0] = (int)numericUpDown1.Value;
            X[1] = (int)numericUpDown2.Value;
            X[2] = (int)numericUpDown3.Value;
            X[3] = (int)numericUpDown4.Value;
            X[4] = (int)numericUpDown5.Value;
            FX[0] = (int)numericUpDown6.Value;
            FX[1] = (int)numericUpDown7.Value;
            FX[2] = (int)numericUpDown8.Value;
            FX[3] = (int)numericUpDown9.Value;
            FX[4] = (int)numericUpDown10.Value;
        }

        void ClearCharts()
        {
            chart1.Series[0].Points.Clear();
            chart2.Series[0].Points.Clear();
            chart3.Series[0].Points.Clear();
            chart4.Series[0].Points.Clear();
            chart5.Series[0].Points.Clear();
            chart1.Series[1].Points.Clear();
            chart2.Series[1].Points.Clear();
            chart3.Series[1].Points.Clear();
            chart4.Series[1].Points.Clear();
            chart5.Series[1].Points.Clear();
        }

        public float[] LagrangeCoeffs()
        {
            int coeffsAmount = X.Length;
            float[] finalCoeff = new float[coeffsAmount];
            float[] c = new float[coeffsAmount];

            for(int i = 0; i < coeffsAmount; i++)
            {
                c[i] = 1;
                for (int j = 0; j < coeffsAmount; j++)
                {
                    if (i == j) continue;

                    c[i] /= (float)(X[i] - X[j]);
                }
                finalCoeff[i] = c[i] * FX[i];
            }

            return finalCoeff;
        }

        float LagrangePoly(float[] coeffs, float point)
        {
            float result = 0;

            for(int i = 0; i < X.Length; i++)
            {
                float currentMult = coeffs[i];
                for(int j = 0; j < X.Length; j++)
                {
                    if (i == j) continue;
                    currentMult *= point - X[j];
                }
                result += currentMult;
            }

            return result;
        }

        public float[] NewtonCoeffs()
        {
            int coeffsAmount = X.Length;
            float[] finalCoeff = new float[coeffsAmount];
            float[][] tempCoeff = new float[coeffsAmount][];

            for (int i = 0; i < coeffsAmount; i++)
            {
                tempCoeff[i] = new float[coeffsAmount];
            }

            for (int i = 0; i < coeffsAmount; i++)
            {
                for (int j = 0; j < coeffsAmount - i - 1; j++)
                {
                    if (i == 0)
                        tempCoeff[i][j] = (FX[j] - FX[j + 1]) / (float)(X[j] - X[j + 1]);
                    else
                        tempCoeff[i][j] = (tempCoeff[i - 1][j] - tempCoeff[i - 1][j + 1]) / (float)(X[j] - X[j + 1 + i]);
                }
                finalCoeff[i] = tempCoeff[i][0];
            }

            return finalCoeff;
        }

        float NewtonPoly(float[] coeffs, float point)
        {
            float result = FX[0];

            for(int i = 0; i < coeffs.Length; i++)
            {
                float currentValue = coeffs[i];
                for (int j = 0; j <= i; j++)
                {
                    currentValue *= (point - X[j]);
                }

                result += currentValue;
            }

            return result;
        }
        int findMaxIndex(float[][] a, int startIndex, int degree)
        {
            int maxIndex = startIndex;

            for (int j = startIndex; j < degree + 1; j++)
            {
                if (a[startIndex][maxIndex] < a[startIndex][j]) maxIndex = j;
            }

            return maxIndex;
        }

        void swapLines(ref float[][] a, ref float[] b, int i, int j)
        {
            float bufB;
            float[] bufA;

            bufA = a[i];
            a[i] = a[j];
            a[j] = bufA;

            bufB = b[i];
            b[i] = b[j];
            b[j] = bufB;
        }

        void makeZeros(ref float[][] a, ref float[] b, int currentLine, int degree)
        {
            for (int j = degree; j >= currentLine; j--)
            {
                if (j != currentLine)
                {
                    float multiplier = a[j][currentLine] / a[currentLine][currentLine];
                    for (int k = degree; k >= currentLine; k--)
                    {
                        a[j][k] -= a[currentLine][k] * multiplier;
                    }
                    b[j] -= b[currentLine] * multiplier;
                }
                else
                {
                    float multiplier = 1 / a[currentLine][currentLine];
                    for (int k = j; k < degree + 1; k++)
                    {
                        a[j][k] *= multiplier;
                    }
                    b[j] *= multiplier;
                }
            }
        }

        float[] gauss(float[][] a, float[] b, int degree)
        {
            float[] result = new float[degree + 1];
            int maxIndex;


            for (int i = 0; i < degree + 1; i++)
            {
                /*Find maximal element in column*/
                maxIndex = findMaxIndex(a, i, degree);

                /*Swap lines*/
                swapLines(ref a, ref b, i, maxIndex);

                /*making zeros below i line*/
                makeZeros(ref a, ref b, i, degree);

            }

            /*solving system*/
            for (int i = degree; i >= 0; i--)
            {
                float sum = 0;
                for (int j = degree; j > i; j--)
                {
                    sum += result[j] * a[i][j];
                }
                result[i] = (b[i] - sum) / a[i][i];
            }

            return result;
        }

        float[] SquaresCoeffs(int degree)
        {
            float[] finalCoeffs = new float[degree + 1];
            float[] C = new float[2 * degree + 1];
            float[] D = new float[degree + 1];
            
            //Calculating coefficients for system of equations
            for (int i = 0; i < C.Length; i++)
            {
                for (int j = 0; j < X.Length; j++)
                {
                    C[i] += (float)Math.Pow(X[j], i);
                }
            }

            for (int i = 0; i < D.Length; i++)
            {
                for (int j = 0; j < X.Length; j++)
                {
                    D[i] += FX[j] * (float)Math.Pow(X[j], i);
                }
            }

            //Create system of equations
            float[][] A = new float[degree + 1][];
            float[] B = new float[degree + 1];
            for (int i = 0; i < degree + 1; i++)
            {
                A[i] = new float[degree + 1];
                for (int j = 0; j < degree + 1; j++)
                {
                    A[i][j] = C[i + j];
                }
                B[i] = D[i];
            }

            finalCoeffs = gauss(A, B, degree);

            return finalCoeffs;
        }

        float SquaresPoly(float[] coeffs, float point)
        {
            float result = 0;

            for (int i = 0; i < coeffs.Length; i++)
            {
                float currentSum = 0;
                currentSum = coeffs[i] * (float)Math.Pow(point, i);

                result += currentSum;
            }

            return result;
        }

        float[] GetUserCoeffs()
        {
            float[] coeffs = new float[5];

            coeffs[0] = float.Parse(textBox1.Text, CultureInfo.InvariantCulture.NumberFormat);
            coeffs[1] = float.Parse(textBox2.Text, CultureInfo.InvariantCulture.NumberFormat);
            coeffs[2] = float.Parse(textBox3.Text, CultureInfo.InvariantCulture.NumberFormat);
            coeffs[3] = float.Parse(textBox4.Text, CultureInfo.InvariantCulture.NumberFormat);
            coeffs[4] = float.Parse(textBox5.Text, CultureInfo.InvariantCulture.NumberFormat);

            return coeffs;
        }

        float UserPoly(float[] userCoeff, float point)
        {
            float result = 0;

            for (int i = 0; i < userCoeff.Length; i++)
            {
                result += userCoeff[i] * (float)Math.Pow(point, i);
            }

            return result;
        }

        void RenderPlot(float[] polyCoeff, 
                        Func<float[], float, float> Poly, 
                        float[] userCoeff,
                        System.Windows.Forms.DataVisualization.Charting.Chart chart,
                        string legend)
        {
            float[] pointsFunc = new float[100];
            float[] pointsUser = new float[100];
            float[] pointsX = new float[100];
            float start = X[0], end = X[X.Length - 1];
            float h = (end - start) / (float)pointsFunc.Length;

            chart.Series[0].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            chart.Series[1].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            chart.Series[0].LegendText = legend;

            for (int i = 0; i < pointsFunc.Length; i++)
            {
                pointsX[i] = start + h * i;
                pointsFunc[i] = Poly(polyCoeff, pointsX[i]);
                pointsUser[i] = UserPoly(userCoeff, pointsX[i]);
                chart.Series[0].Points.AddXY(pointsX[i], pointsFunc[i]);
                chart.Series[1].Points.AddXY(pointsX[i], pointsUser[i]);
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            GetFunctionValues();
            ClearCharts();

            float[] lagrCoeff = LagrangeCoeffs();
            float[] newtCoeff = NewtonCoeffs();
            float[] squares1Coeff = SquaresCoeffs(1);
            float[] squares2Coeff = SquaresCoeffs(2);
            float[] squares3Coeff = SquaresCoeffs(3);
            float[] userCoeff = GetUserCoeffs();

            Func<float[], float, float> LagrPoly = LagrangePoly;
            Func<float[], float, float> NewtPoly = NewtonPoly;
            Func<float[], float, float> SqrPoly = SquaresPoly;

            RenderPlot(lagrCoeff, LagrPoly, userCoeff, chart1, "Метод Лагранжа");
            RenderPlot(newtCoeff, NewtPoly, userCoeff, chart2, "Метод Ньютона");
            RenderPlot(squares1Coeff, SqrPoly, userCoeff, chart3, "Метод наименьших квадратов 1-й степени");
            RenderPlot(squares2Coeff, SqrPoly, userCoeff, chart4, "Метод наименьших квадратов 2-й степени");
            RenderPlot(squares3Coeff, SqrPoly, userCoeff, chart5, "Метод наименьших квадратов 3-й степени");
        }

        private bool CheckCharacter(char ch, string text)
        {
            bool result = false;

            result = !Char.IsDigit(ch)
                && ch != 8
                && (ch != '.' || text.Contains('.'))
                && (ch != '-' || text.Length != 0);

            return result;
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ch = e.KeyChar;
            e.Handled = CheckCharacter(ch, textBox1.Text);
        }

        private void textBox2_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ch = e.KeyChar;
            e.Handled = CheckCharacter(ch, textBox2.Text);
        }

        private void textBox3_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ch = e.KeyChar;
            e.Handled = CheckCharacter(ch, textBox3.Text);
        }

        private void textBox4_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ch = e.KeyChar;
            e.Handled = CheckCharacter(ch, textBox4.Text);
        }

        private void textBox5_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ch = e.KeyChar;
            e.Handled = CheckCharacter(ch, textBox5.Text);
        }
    }
}

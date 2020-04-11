using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Spline
{
    public partial class Form1 : Form
    {
        int[] X = new int[5];
        int[] FX = new int[5];

        public Form1()
        {
            InitializeComponent();
        }

        private void GetFunctionValues()
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
                if (i != maxIndex)
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

        float[][] GetCubicSplineCoeffs()
        {
            float[][] functions = new float[X.Length-1][];
            for(int i = 0; i < X.Length - 1; i++)
            {
                functions[i] = new float[X.Length - 1];
            }

            //Get values a
            for(int i = 0; i < functions.Length; i++)
            {
                functions[i][0] = FX[i];
            }

            //Get values c
            functions[0][2] = 0;
            int systemSize = functions.Length - 1;
            int h = 0;
            float[][] systemMatrix = new float[systemSize][];
            float[] systemSolution = new float[systemSize];
            float[] systemRightSide = new float[systemSize]; 
            for (int i = 0; i < systemSize; i++)
            {
                h = X[i + 2] - X[i + 1];
                systemMatrix[i] = new float[systemSize];

                for (int j = 0; j < systemSize; j++)
                {
                    if (i == j)
                    {
                        systemMatrix[i][j] = 4 * h;
                    }
                    else if (i + 1 == j || i - 1 == j)
                    {
                        systemMatrix[i][j] = h;
                    }
                    else
                    {
                        systemMatrix[i][j] = 0;
                    }
                }

                systemRightSide[i] = 3 * (FX[i+2] - 2 * FX[i+1] + FX[i]) / (float)h;
            }
            systemSolution = gauss(systemMatrix, systemRightSide, systemRightSide.Length - 1);

            for (int i = 1; i < functions.Length; i++)
            {
                functions[i][2] = systemSolution[i - 1];
            }

            //Get values b and d
            for (int i = 0; i < functions.Length; i++)
            {
                h = X[i + 1] - X[i];
                if (i != functions.Length - 1 && i != 0)
                {
                    functions[i][1] = ((FX[i + 1] - FX[i]) / (float)h) - ((functions[i + 1][2] + 2 * functions[i][2]) * h / (float)3.0);
                    functions[i][3] = (functions[i + 1][2] - functions[i][2]) / (float)(3 * h);
                }
                else if (i == functions.Length - 1)
                {
                    functions[i][1] = ((FX[i + 1] - FX[i]) / (float)h) - (2 * functions[i][2] * h / (float)3.0);
                    functions[i][3] = -functions[i][2] / (float)(3 * h);
                }
                else
                {
                    functions[i][1] = ((FX[i + 1] - FX[i]) / (float)h) - (functions[i + 1][2] * h / (float)3.0);
                    functions[i][3] = functions[i + 1][2] / (float)(3 * h);
                }
            }

            return functions;
        }

        int GetIntervalNumber(float point)
        {
            for (int i = 1; i < X.Length; i++)
            {
                if (point < (float)X[i])
                {
                    return i-1;
                }
            }
            return X.Length-1;
        }

        float SplineFunction(float[][] coeff, float point)
        {
            float result = 0;
            int interval = GetIntervalNumber(point);

            result = coeff[interval][0];
            for (int i = 1; i < coeff[interval].Length; i++)
            {
                result += coeff[interval][i] * (float)Math.Pow((point - X[interval]), i);
            }

            return result;
        }

        float DSplineFunction(float[][] coeff, float point)
        {
            float result = 0;
            float start = X[0], end = X[X.Length - 1];
            float h = (end - start) / (float)100;

            result = (SplineFunction(coeff, point + h) - SplineFunction(coeff, point - h)) / (2*h);

            return result;
        }

        float DDSplineFunction(float[][] coeff, float point)
        {
            float result = 0;
            float start = X[0], end = X[X.Length - 1];
            float h = (end - start) / (float)100;
            
            result = (SplineFunction(coeff, point + h) - 2 * SplineFunction(coeff, point) + SplineFunction(coeff, point - h)) / (h*h);

            return result;
        }

        void RenderPlot(float[][] coeff,
                        float start,
                        float end,
                        Func<float[][], float, float> Poly,
                        System.Windows.Forms.DataVisualization.Charting.Chart chart,
                        string legend)
        {
            float[] pointsFunc = new float[100];
            float[] pointsX = new float[100];
            float h = (end - start) / (float)pointsFunc.Length;

            chart.Series[0].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            chart.Series[0].LegendText = legend;

            for (int i = 0; i < pointsFunc.Length; i++)
            {
                pointsX[i] = start + h * i;
                pointsFunc[i] = Poly(coeff, pointsX[i]);
                chart.Series[0].Points.AddXY(pointsX[i], pointsFunc[i]);
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            GetFunctionValues();
            ClearCharts();

            float[][] splineCoeffs = GetCubicSplineCoeffs();

            Func<float[][], float, float> SplineFunc = SplineFunction;
            Func<float[][], float, float> DSplineFunc = DSplineFunction;
            Func<float[][], float, float> DDSplineFunc = DDSplineFunction;

            RenderPlot(splineCoeffs, X[0], X[X.Length - 1], SplineFunc, chart1, "Кубический сплайн");
            RenderPlot(splineCoeffs, X[0] + (float)0.15, X[X.Length - 1] - (float)0.15, DSplineFunc, chart2, "Первая производная");
            RenderPlot(splineCoeffs, X[0] + (float)0.15, X[X.Length - 1] - (float)0.15, DDSplineFunc, chart3, "Вторая производная");
        }
    }
}

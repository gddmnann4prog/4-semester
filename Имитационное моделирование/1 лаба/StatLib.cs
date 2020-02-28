using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WindowsFormsApp8
{
    public class StatLib
    {
        double expect = 0.5;
        double disp = 1/12.0;

        double[] selection = new double[40];

        double PartialSum(int k)
        {
            double result = 0;

            for (int i = 0; i < k; i++)
            {
                result += selection[i];
            }

            return result;
        }

        double[] CLTExpression()
        {
            double[] result = new double[40];

            for (int i = 0; i < 40; i++)
            {
                result[i] = PartialSum(i) - i * expect;
                result[i] /= Math.Sqrt(i*disp);
            }

            return result;
        }

        double[] BoxMullerExpression()
        {
            double[] result = new double[40];

            for (int i = 0; i < 20; i++)
            {
                result[2 * i] = Math.Sqrt(-2 * Math.Log(selection[2 * i])) * Math.Cos(2 * Math.PI * selection[2 * i + 1]);
                result[2 * i + 1] = Math.Sqrt(-2 * Math.Log(selection[2 * i])) * Math.Sin(2 * Math.PI * selection[2 * i + 1]);
            }

            return result;
        }
    }
}

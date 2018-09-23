using mjohansen.labview.email;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        EmailManager emailManager;


        public Form1()
        {
            InitializeComponent();
            emailManager = new EmailManager();
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            decimal z1 = numericUpDown1.Value;
            decimal z2 = numericUpDown2.Value;
            decimal z3 = numericUpDown3.Value;
            decimal z4 = numericUpDown4.Value;

            decimal avg = ((z1 + z2 + z3 + z4) / 4);
            numericUpDown5.Value = avg; 



        }

        private void button1_Click(object sender, EventArgs e)
        {
            decimal z1 = numericUpDown1.Value;
            decimal z2 = numericUpDown2.Value;
            decimal z3 = numericUpDown3.Value;
            decimal z4 = numericUpDown4.Value;
            decimal z5 = numericUpDown5.Value;
            emailManager.SendPushData((float)z1, (float)z2, (float)z3, (float)z4, (float)z5);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            emailManager.SendPushAlarm(textBox1.Text);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;

namespace Web3._7
{
    public class EmailMessageSender :IMessageSender
    {
        private string text;
        public EmailMessageSender(HttpContext context)
        {
            text = context.Session.GetString("text");
        }
        public string Send()
        {
            if (text == null)
            {
                return "text empty";
            }
            return text;
        }
    }
}
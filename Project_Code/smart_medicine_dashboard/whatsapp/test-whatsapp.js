require("dotenv").config();
const twilio = require("twilio");

const accountSid = process.env.TWILIO_ACCOUNT_SID;
const authToken = process.env.TWILIO_AUTH_TOKEN;

const client = twilio(
  accountSid,
  authToken
);

async function sendTestMessage() {

  try {

    const message =
      await client.messages.create({

        from:
          "whatsapp:+14155238886",

        to:
          "whatsapp:+919272556195",

        body:
          "🚀 Smart Medicine IoT\n\nWhatsApp integration test successful!"
      });

    console.log(
      "Message sent successfully!"
    );

    console.log(
      "Message SID:",
      message.sid
    );

  } catch (error) {

    console.error(
      "WhatsApp message failed:"
    );

    console.error(
      error.message
    );
  }
}

sendTestMessage();
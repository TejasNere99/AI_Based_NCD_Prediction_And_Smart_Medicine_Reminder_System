const express = require("express");
const twilio = require("twilio");
require("dotenv").config();

const app = express();

const PORT = 5000;

// =====================================================
// TWILIO
// =====================================================

const accountSid =
  process.env.TWILIO_ACCOUNT_SID;

const authToken =
  process.env.TWILIO_AUTH_TOKEN;

if (!accountSid || !authToken) {

  console.error(
    "ERROR: Twilio credentials are missing."
  );

  console.error(
    "Set TWILIO_ACCOUNT_SID and TWILIO_AUTH_TOKEN first."
  );

  process.exit(1);
}

const client =
  twilio(
    accountSid,
    authToken
  );

// =====================================================
// CONFIG
// =====================================================

// Twilio WhatsApp Sandbox sender
const TWILIO_WHATSAPP_NUMBER =
  "whatsapp:+14155238886";

// YOUR WhatsApp number
const USER_WHATSAPP_NUMBER =
  "whatsapp:+919272556195";

// =====================================================
// MIDDLEWARE
// =====================================================

app.use(
  express.json()
);

app.use(
  express.urlencoded({
    extended: true
  })
);

// =====================================================
// ROOT
// =====================================================

app.get("/", (req, res) => {

  res.send(
    "Smart Medicine WhatsApp Backend ONLINE"
  );
});

// =====================================================
// MEDICINE EVENT
// =====================================================

app.post(
  "/api/medicine-event",
  async (req, res) => {

    try {

      const {
        slot,
        medicine,
        status,
        scheduledTime,
        takenTime
      } = req.body;

      console.log();
      console.log(
        "================================"
      );

      console.log(
        "      MEDICINE EVENT"
      );

      console.log(
        "================================"
      );

      console.log(
        "Slot:",
        slot
      );

      console.log(
        "Medicine:",
        medicine
      );

      console.log(
        "Status:",
        status
      );

      console.log(
        "Scheduled:",
        scheduledTime
      );

      console.log(
        "Taken At:",
        takenTime
      );

      console.log(
        "================================"
      );

      // -------------------------------------------------
      // Validate
      // -------------------------------------------------

      if (
        !slot ||
        !medicine ||
        !status
      ) {

        return res.status(400).json({

          success: false,

          error:
            "slot, medicine and status are required"

        });
      }

      // -------------------------------------------------
      // Build WhatsApp message
      // -------------------------------------------------

      let messageBody = "";

      if (
        status === "TAKEN"
      ) {

        messageBody =
`✅ MEDICINE TAKEN

Medicine: ${medicine}
Slot: ${slot}
Scheduled Time: ${scheduledTime || "--:--"}
Taken At: ${takenTime || "--:--:--"}

Smart Medicine IoT System`;

      }

      else if (
        status === "NOT_TAKEN"
      ) {

        messageBody =
`⚠️ MEDICINE NOT TAKEN

Medicine: ${medicine}
Slot: ${slot}
Scheduled Time: ${scheduledTime || "--:--"}

Medicine was not detected within
the allowed time.

Smart Medicine IoT System`;

      }

      else if (
        status === "DISPENSING"
      ) {

        messageBody =
`💊 MEDICINE REMINDER

Medicine: ${medicine}
Slot: ${slot}
Scheduled Time: ${scheduledTime || "--:--"}

Please take your medicine.

Smart Medicine IoT System`;

      }

      else {

        return res.status(400).json({

          success: false,

          error:
            "Unsupported medicine status"

        });
      }

      // -------------------------------------------------
      // Send WhatsApp
      // -------------------------------------------------

      const message =
        await client.messages.create({

          from:
            TWILIO_WHATSAPP_NUMBER,

          to:
            USER_WHATSAPP_NUMBER,

          body:
            messageBody

        });

      console.log();

      console.log(
        "WhatsApp message sent!"
      );

      console.log(
        "Message SID:",
        message.sid
      );

      // -------------------------------------------------
      // Response
      // -------------------------------------------------

      res.json({

        success: true,

        messageSid:
          message.sid,

        status:
          status

      });

    }

    catch (error) {

      console.error();

      console.error(
        "WhatsApp sending failed:"
      );

      console.error(
        error.message
      );

      res.status(500).json({

        success: false,

        error:
          error.message

      });
    }
  }
);

// =====================================================
// SERVER
// =====================================================

app.listen(
  PORT,
  "0.0.0.0",
  () => {

    console.log();
    console.log(
      "================================"
    );

    console.log(
      " Smart Medicine WhatsApp Server"
    );

    console.log(
      "================================"
    );

    console.log(
      `Server running on port ${PORT}`
    );

    console.log();
  }
);
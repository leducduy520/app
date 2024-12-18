// Import necessary modules
const express = require("express");
const mongoose = require("mongoose");
const bodyParser = require("body-parser");
const dotenv = require("dotenv");
const fs = require("fs"); // Import filesystem module
const { detectAuthMethod, AuthMethods } = require("./utils/authUtils"); // Import utility for authentication detection
const recordsRoutes = require("./routes/recordsRoutes");

// Load environment variables
dotenv.config();

const app = express();
const port = process.env.PORT || 3000; // Set port from environment or use 3000 as default

// Middleware to parse JSON requests
app.use(bodyParser.json());

// Read database and collection names from environment variables or CLI arguments
const args = process.argv.slice(2);
const dbName = args[0] || process.env.MONGODB_NAME || "default_database"; // Database name
const collectionName =
  args[1] || process.env.MONGODB_COLL || "default_collection"; // Collection name
console.log(`Using database: ${dbName}, collection: ${collectionName}`);

// Check if 'MONGODB_CERT_ENCODE' environment variable exists
if (process.env.MONGODB_CERT_ENCODE) {
  console.log("Environment variable 'MONGODB_CERT_ENCODE' detected.");

  // Path for the base64-encoded file and decoded file
  const encodedFilePath = "X509-cert.pem.base64";
  const decodedFilePath = process.env.MONGODB_CERT;

  try {
    // Write the base64-encoded content to a file
    fs.writeFileSync(encodedFilePath, process.env.MONGODB_CERT_ENCODE, "utf8");
    console.log(`Encoded certificate written to ${encodedFilePath}`);

    // Decode the base64 file and save it to the decoded file path
    const encodedContent = fs.readFileSync(encodedFilePath, "utf8");
    const decodedContent = Buffer.from(encodedContent, "base64").toString("utf8");
    fs.writeFileSync(decodedFilePath, decodedContent, "utf8");
    console.log(`Decoded certificate saved to ${decodedFilePath}`);
  } catch (err) {
    console.error("Error processing 'MONGODB_CERT_ENCODE':", err.message);
    process.exit(1); // Exit with an error code
  }
} else {
  console.log("Environment variable 'MONGODB_CERT_ENCODE' is not set.");
}

// Detect the authentication method from MongoDB URI
const auth_method = detectAuthMethod(process.env.MONGODB_URI);
console.log(`Authentication method: ${auth_method}`);

// MongoDB connection options
const connectionOptions = {
  serverApi: { version: "1", strict: true, deprecationErrors: true }, // Enforce MongoDB driver options
  appName: "app", // Application name for MongoDB telemetry
  w: "majority", // Ensure majority writes
  retryWrites: true, // Retry writes in case of transient errors
  dbName: dbName, // Database name
};

// Configure connection options based on detected authentication method
switch (auth_method) {
  case AuthMethods.X509:
    {
      connectionOptions.tls = true; // Enable TLS for X.509
      connectionOptions.tlsCertificateKeyFile = process.env.MONGODB_CERT; // Path to X.509 certificate
      connectionOptions.tlsCAFile = process.env.MONGODB_CA;
      connectionOptions.authSource = "$external"
      connectionOptions.authMechanism ="MONGODB-X509";
      console.log("Detected Authentication Method: X.509");
    }
    break;
  case AuthMethods.PASSWORD:
    console.log("Detected Authentication Method: Password-Based");
    break;
  case AuthMethods.NONE:
    console.log("Detected Authentication Method: No Authentication");
    break;
  case AuthMethods.INVALID:
    console.error("Invalid MongoDB URI");
    process.exit(1); // Exit process if URI is invalid
  default:
    console.error("Unknown Authentication Method");
    process.exit(1); // Exit process if unknown method is detected
}

// Connect to MongoDB using the configured options
mongoose.connect(process.env.MONGODB_URI, connectionOptions);

const db = mongoose.connection;

// Handle connection errors
db.on("error", console.error.bind(console, "MongoDB connection error:"));

// Log successful connection
db.once("open", () => {
  console.log("Connected to MongoDB successfully!");
});

// Mount the router with base path '/records'
app.use("/records", recordsRoutes);

// Start the Express server
app.listen(port, () => {
  console.log(`API running on http://localhost:${port}`);
});

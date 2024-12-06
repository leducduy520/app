const express = require("express");
const mongoose = require("mongoose");
const bodyParser = require("body-parser");
const dotenv = require("dotenv");
const createDynamicModel = require("./models/Histories");

dotenv.config();

const app = express();
const port = process.env.PORT || 3000;

// Middleware
app.use(bodyParser.json());

// Read database and collection names from environment or CLI arguments
const args = process.argv.slice(2);
const dbName = args[0] || process.env.MONGODB_NAME || "default_database";
const collectionName =
  args[1] || process.env.MONGODB_COLL || "default_collection";
console.log(`Using database: ${dbName}, collection: ${collectionName}`);

// MongoDB connection using X.509
mongoose.connect(process.env.MONGODB_URI, {
  tls: true,
  tlsCertificateKeyFile: process.env.MONGODB_CA,
  authMechanism: "MONGODB-X509",
  authSource: "$external",
  dbName: dbName, // Your database name
});

const db = mongoose.connection;
db.on("error", console.error.bind(console, "MongoDB connection error:"));
db.once("open", () => {
  console.log("Connected to MongoDB using X.509 authentication");
});

// Create a dynamic model for the chosen collection
const Main = createDynamicModel(collectionName);

// API Endpoints

// Add a new record
app.post("/records", async (req, res) => {
  try {
    const newRecord = new Main({
      _id: req.body._id,
      history: req.body.history || [],
    });
    await newRecord.save();
    res.status(201).json(newRecord); // Return the newly created record
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});

// Get all records
app.get("/records", async (req, res) => {
  try {
    const records = await Main.find();
    res.json(records);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Get a specific record by ID
app.get("/records/:id", async (req, res) => {
  try {
    const record = await Main.findById(req.params.id);
    if (!record) return res.status(404).json({ error: "Record not found" });
    res.json(record);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Add a history entry
app.post("/records/:id/history", async (req, res) => {
  try {
    const record = await Main.findById(req.params.id);
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = req.body.sid
      ? new mongoose.Types.ObjectId(req.body.sid)
      : new mongoose.Types.ObjectId();

    const newHistory = {
      sid: sid,
      modules: req.body.modules || [],
    };

    record.history.push(newHistory);
    await record.save();
    res.status(201).json(newHistory);
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});

// Update a history entry
app.put("/records/:id/history/:sid", async (req, res) => {
  try {
    const record = await Main.findById(req.params.id);
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = new mongoose.Types.ObjectId(req.params.sid);

    const historyItem = record.history.find((h) => h.sid.equals(sid));
    if (!historyItem)
      return res.status(404).json({ error: "History item not found" });

    Object.assign(historyItem, req.body);
    await record.save();
    res.json(historyItem);
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});

// Delete a history entry
app.delete("/records/:id/history/:sid", async (req, res) => {
  try {
    const record = await Main.findById(req.params.id);
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = new mongoose.Types.ObjectId(req.params.sid);

    const historyIndex = record.history.findIndex((h) => h.sid.equals(sid));
    if (historyIndex === -1)
      return res.status(404).json({ error: "History item not found" });

    record.history.splice(historyIndex, 1);
    await record.save();
    res.status(204).send();
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Retrieve all history entries
app.get("/records/:id/history", async (req, res) => {
  try {
    const record = await Main.findById(req.params.id);
    if (!record) return res.status(404).json({ error: "Record not found" });

    res.json(record.history);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Retrieve a specific history entry
app.get("/records/:id/history/:sid", async (req, res) => {
  try {
    const record = await Main.findById(req.params.id);
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = new mongoose.Types.ObjectId(req.params.sid);

    const historyItem = record.history.find((h) => h.sid.equals(sid));
    if (!historyItem)
      return res.status(404).json({ error: "History item not found" });

    res.json(historyItem);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Start the server
app.listen(port, () => {
  console.log(`API running on http://localhost:${port}`);
});

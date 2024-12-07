const express = require("express");
const mongoose = require("mongoose");
const createDynamicModel = require("../models/moduleapp"); // Import dynamic model creation function

const router = express.Router();

// Dynamically create the model
const collectionName = process.env.MONGODB_COLL || "default_collection";
const moduleapp = createDynamicModel(collectionName);

// Add a new record
router.post("/", async (req, res) => {
  try {
    const newRecord = new moduleapp({
      _id: req.body._id, // Specify ID if provided
      history: req.body.history || [], // Default to empty array if history not provided
    });
    await newRecord.save(); // Save record to database
    res.status(201).json(newRecord); // Return the newly created record
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});

// Get all records
router.get("/", async (req, res) => {
  try {
    const records = await moduleapp.find(); // Retrieve all records
    res.json(records);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Get a specific record by ID
router.get("/:id", async (req, res) => {
  try {
    const record = await moduleapp.findById(req.params.id); // Find record by ID
    if (!record) return res.status(404).json({ error: "Record not found" });
    res.json(record);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Add a history entry to a specific record
router.post("/:id/history", async (req, res) => {
  try {
    const record = await moduleapp.findById(req.params.id); // Find record by ID
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = req.body.sid
      ? new mongoose.Types.ObjectId(req.body.sid)
      : new mongoose.Types.ObjectId(); // Generate a new ID if not provided

    const newHistory = {
      sid: sid, // Sub-document ID
      modules: req.body.modules || [], // Modules array
    };

    record.history.push(newHistory); // Add new history to record
    await record.save(); // Save changes to database
    res.status(201).json(newHistory); // Return the new history entry
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});

// Update a history entry in a specific record
router.put("/:id/history/:sid", async (req, res) => {
  try {
    const record = await moduleapp.findById(req.params.id); // Find record by ID
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = new mongoose.Types.ObjectId(req.params.sid); // Get sub-document ID

    const historyItem = record.history.find((h) => h.sid.equals(sid)); // Find history entry
    if (!historyItem)
      return res.status(404).json({ error: "History item not found" });

    Object.assign(historyItem, req.body); // Update fields
    await record.save(); // Save changes to database
    res.json(historyItem);
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});

// Delete a history entry
router.delete("/:id/history/:sid", async (req, res) => {
  try {
    const record = await moduleapp.findById(req.params.id); // Find record by ID
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = new mongoose.Types.ObjectId(req.params.sid); // Get sub-document ID

    const historyIndex = record.history.findIndex((h) => h.sid.equals(sid)); // Find history index
    if (historyIndex === -1)
      return res.status(404).json({ error: "History item not found" });

    record.history.splice(historyIndex, 1); // Remove history entry
    await record.save(); // Save changes to database
    res.status(204).send(); // Return no content
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Retrieve all history entries from a record
router.get("/:id/history", async (req, res) => {
  try {
    const record = await moduleapp.findById(req.params.id); // Find record by ID
    if (!record) return res.status(404).json({ error: "Record not found" });

    res.json(record.history); // Return history array
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Retrieve a specific history entry from a record
router.get("/:id/history/:sid", async (req, res) => {
  try {
    const record = await moduleapp.findById(req.params.id); // Find record by ID
    if (!record) return res.status(404).json({ error: "Record not found" });

    const sid = new mongoose.Types.ObjectId(req.params.sid); // Get sub-document ID

    const historyItem = record.history.find((h) => h.sid.equals(sid)); // Find history entry
    if (!historyItem)
      return res.status(404).json({ error: "History item not found" });

    res.json(historyItem); // Return history item
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Add a module to a specific history entry
router.post("/:id/history/:sid/modules", async (req, res) => {
  try {
    const { id, sid } = req.params;

    // Validate the module object before adding
    if (!req.body.module_id || !req.body.module_path) {
      return res
        .status(400)
        .json({ error: "module_id and module_path are required" });
    }

    // Find the record by ID
    const record = await moduleapp.findById(id);
    if (!record) {
      return res.status(404).json({ error: "Record not found" });
    }

    // Find the history entry by sid
    const historyItem = record.history.find((h) =>
      h.sid.equals(new mongoose.Types.ObjectId(sid))
    );
    if (!historyItem) {
      return res.status(404).json({ error: "History item not found" });
    }

    // Add the new module to the modules array
    historyItem.modules.push(req.body);

    // Save the updated record to the database
    await record.save();

    res.status(201).json(req.body); // Return the newly added module
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// More routes...

module.exports = router;

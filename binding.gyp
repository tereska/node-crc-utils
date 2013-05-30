{
  "targets": [
    {
        "target_name": "crc32",
        "sources": [ "src/crc32.cc" ]
    },
    {
        "target_name": "after_build",
        "type": "none",
        "dependencies": [
            "crc32"
        ],
        "actions": [
            {
                "action_name": "symlink",
                "inputs": [
                    "<@(PRODUCT_DIR)/crc32.node"
                ],
                "outputs": [
                    "<(module_root_dir)/bin/crc32.node"
                ],
                "action": ["ln", "-s", "<@(PRODUCT_DIR)/crc32.node", "<(module_root_dir)/bin/crc32.node"]
            }
        ]
    }
  ]
}


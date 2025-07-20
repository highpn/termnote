int poll_stdin();        // Non-blocking check if input is ready
int poll_file_changes(); // (Optional): use `poll()` or fallback to reload

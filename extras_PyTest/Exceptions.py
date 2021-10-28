# Model File Errors
class ModelFileReadError(Exception):
    pass
class ModelFileTransferError(Exception):
    pass
class ModelFileTypeError(Exception):
    pass
class ModelFileUnitError(Exception):
    pass

# Model Errors
class ModelValidityError(Exception):
    pass
class ModelNoShapesError(Exception):
    pass
class ModelMultipleShapesError(Exception):
    pass

# Stock Errors
class StockNotAvailableError(Exception):
    """Raised when not enough stock is available."""
    def __init__(self, message):
        self.message = message

    def __repr__(self):
        return self.message

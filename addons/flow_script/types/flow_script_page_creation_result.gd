@tool
class_name FlowScriptPageCreationResult
extends RefCounted


enum PageCreationErrorCode {
	OK = 0,
	MAX_PAGES_EXCEEDED = 1,
	ID_INVALID = 2,
}


var error := PageCreationErrorCode.OK: set = set_error, get = get_error
var page: FlowPage = null: set = set_page, get = get_page


func set_error(p_error_code: PageCreationErrorCode) -> void:
	error = p_error_code

func get_error() -> PageCreationErrorCode:
	return error


func set_page(p_page_reference: FlowPage) -> void:
	page = p_page_reference

func get_page() -> FlowPage:
	return page

"""Retain a UObject listener instead of a delegate reference into a destroyed actor."""
import unreal, builtins

@unreal.uclass()
class HodgeHitReactionValidationObserver(unreal.Object):
    feedback_key = unreal.uproperty(str)

    @unreal.ufunction(params=[unreal.Vector])
    def receive_feedback(self, direction):
        context = getattr(builtins, 'HODGE_REACTION_TEST', None)
        if context:
            key = self.get_editor_property('feedback_key')
            context['feedback'][key] = context['feedback'].get(key, 0) + 1

def bind(pawn, context):
    key = pawn.get_path_name()
    context['feedback'].setdefault(key, 0)
    observer = unreal.new_object(HodgeHitReactionValidationObserver)
    observer.set_editor_property('feedback_key', key)
    reaction = pawn.get_component_by_class(unreal.HodgeHitReactionComponent)
    reaction.on_light_feedback.add_function(observer, 'receive_feedback')
    context.setdefault('feedback_observers', []).append((reaction, observer))

def unbind(context):
    for reaction, observer in context.get('feedback_observers', []):
        try:
            if unreal.SystemLibrary.is_valid(reaction): reaction.on_light_feedback.remove_function(observer, 'receive_feedback')
        except TypeError:
            # UE has already invalidated the Python wrapper for a destroyed target.
            pass
    context['feedback_observers'] = []
